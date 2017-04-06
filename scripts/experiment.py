"""
Script for generating experiment output.
"""

import argparse
import os
from subprocess import call
from multiprocessing import Pool
from utilities import Directory, Commands, Experiment
import program_generator as pg


def create_folders(experiment):
    """Creates the folders necessary for experiment output.
    We run this serially to avoid race conditions.

    Parameters
    ----------
    experiment : Experiment
        The experiment parameters

    Returns
    -------
    Nothing
    """
    for parameters in experiment:
        (c, m, n), program, program_seed, algorithm, algorithm_seed \
            = parameters
        algorithm_dir = os.path.join(Directory.OUTPUT_DIR,
                                     experiment.name,
                                     program,
                                     program_seed,
                                     algorithm)
        if not os.path.exists(algorithm_dir):
            os.makedirs(algorithm_dir)


def batch_run(experiment, num_threads=1):
    """
    Executes an experiment in parallel, specified by the number of threads.

    Parameters
    ----------
    experiment : Experiment
        The experiment parameters
    num_threads : int
        The number of threads to use in the multiprocessing pool

    Returns
    -------
    Nothing
    """
    pool = Pool(num_threads)
    create_folders(experiment)
    for _ in pool.imap(compute_embedding, experiment):
        pass


def compute_embedding(parameters, debug=False):
    """Compute an embedding using one set of parameters from the experiment.
    Computation is done with a call to the C++ driver.
    Enable debug to run Valgrind.

    Parameters
    ----------
    parameters : list
        A single instance of the experiment parameters
    debug : bool
        Run Valgrind with the C++ commands

    Returns
    -------
    Nothing
    """

    # Unpack the parameters
    (c, m, n), program, program_seed, algorithm, algorithm_seed = parameters

    # Build the file paths
    program_file = os.path.join(Directory.INPUT_DIR,
                                program,
                                program_seed + ".graph")
    output_file = os.path.join(Directory.OUTPUT_DIR,
                               experiment.name,
                               program,
                               program_seed,
                               algorithm,
                               algorithm_seed)

    # Construct the C++ driver call

    command = []
    command += Commands.TIMEOUT
    command += Commands.DRIVER
    command += Commands.algorithm(algorithm)
    command += Commands.chimera(c, m, n)
    command += Commands.program_file(program_file)
    command += Commands.output_file(output_file)

    if(algorithm == "cmr"):
        hardware_file = os.path.join(Directory.INPUT_DIR,
                                     "hardware",
                                     "chimera_%s_%s_%s.graph" % (c, m, n))
        command += Commands.hardware_file(hardware_file)
        command += Commands.seed(algorithm_seed)

    if(algorithm in ["fast-oct",
                     "fast-oct-reduce",
                     "hybrid-oct",
                     "hybrid-oct-reduce",
                     "oct-fast",
                     "oct-fast-native"]):
        command += Commands.seed(algorithm_seed)
        command += Commands.repeats(10000)
    if debug:
        command = Commands.valgrind + command

    # Execute the C++ driver call
    call(command)


def generate_input(experiment):
    """Generates the input program/hardware graphs needed for an experiment.

    Parameters
    ----------
    experiment : Experiment
        The experiment parameters

    Returns
    -------
    Nothing
    """

    # Generate harware
    for hardware in experiment.hardware:
        pg.write_chimera(*[int(x) for x in hardware])

    # Generate programs
    for program in experiment.programs:
        program_type, vertices, density = program.split("_")
        vertices = int(vertices)
        for seed in experiment.program_seeds:
            generator = pg.generator_lookup[program_type]
            generator(vertices, density, seed=int(seed))


if __name__ == "__main__":
    """Main method for CLI interaction.

    Parameters
    ----------
    experiment_filename : string
        The experiment config filename
    num_threads : int
        The number of CPU threads to use

    Returns
    -------
    Nothing
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("experiment_filename",
                        help="The experiment config file name.",
                        type=str)
    parser.add_argument("num_threads",
                        help="The number of compute threads used.",
                        type=int)
    args = parser.parse_args()

    # Build an experiment from the first CLI argument
    experiment = Experiment(args.experiment_filename)

    print("Running experiment '{}' with".format(experiment.name))
    print(" * {} programs".format(len(experiment.programs) *
                                 len(experiment.program_seeds)))
    print(" * {} embedding algorithms".format(len(experiment.algorithms)))
    print(" * {} algorithm seeds".format(len(experiment.algorithm_seeds)))
    print("  using {} CPU cores.".format(args.num_threads))

    # Generate the input and run the experiment
    print("Generating input graphs")
    generate_input(experiment)

    print("Running experiment")
    batch_run(experiment, num_threads=args.num_threads)
    print("Experiment finished!")
