"""
Various utilities, mostly for standarizing input/output.
"""


import os
from itertools import product
import pandas as pd


class Directory(object):
    """Class keeps the folder locations.
    """
    INPUT_DIR = "data/input"
    OUTPUT_DIR = "data/output"
    CONFIG_DIR = "data/config"
    PLOTS_DIR = "data/plots"


class Commands(object):
    """
    Specifies interface to the C++ driver.
    """

    # Time out after 60 minutes, kill after 10 more seconds
    TIMEOUT = ['timeout', '-k', '10s', '60m']

    # Run Valgrind for debugging.
    # Remember to compile with -g instead of -O2
    VALGRIND = ["valgrind", "--leak-check=full", "--track-origins=yes"]

    # Run the driver and start the algorithm flag
    DRIVER = ["./embedding/driver"]

    # Flags for the different options:
    FLAGS = {
        "algorithm": ["-a"],
        "chimera_L": ["-c"],
        "chimera_M": ["-m"],
        "chimera_N": ["-n"],
        "program_file": ["-pfile"],
        "output_file": ["-o"],
        "hardware_file": ["-hfile"],
        "seed": ["-s"],
        "repeats": ["-r"]
    }

    @staticmethod
    def algorithm(algorithm):
        """Produces the CLI commands for an algorithm

        Parameters
        ----------
        algorithm : string
            The algorithm name

        Returns
        -------
        list
            The commands generated
        """
        return Commands.FLAGS["algorithm"] + [algorithm]

    @staticmethod
    def chimera(L, M, N):
        """Produces the CLI commands for Chimera parameters

        Parameters
        ----------
        L : int
            The Chimera cell height
        M : int
            The number of Chimera rows
        N : int
            The number of Chimera columns

        Returns
        -------
        list
            The commands generated
        """
        return Commands.FLAGS["chimera_L"] + [str(L)]\
            + Commands.FLAGS["chimera_M"] + [str(M)]\
            + Commands.FLAGS["chimera_N"] + [str(N)]

    @staticmethod
    def program_file(filename):
        """Produces the CLI commands for program filename

        Parameters
        ----------
        filename : string
            The program filename

        Returns
        -------
        list
            The commands generated
        """
        return Commands.FLAGS["program_file"] + [str(filename)]

    @staticmethod
    def output_file(filename):
        """Produces the CLI commands for the output folder

        Parameters
        ----------
        output_file : string
            The output folder

        Returns
        -------
        list
            The commands generated
        """
        return Commands.FLAGS["output_file"] + [str(filename)]

    @staticmethod
    def hardware_file(filename):
        """Produces the CLI commands for the hardware file

        Parameters
        ----------
        filename : string
            The hardware filename

        Returns
        -------
        list
            The commands generated
        """
        return Commands.FLAGS["hardware_file"] + [str(filename)]

    @staticmethod
    def seed(seed):
        """Produces the CLI commands for a seed

        Parameters
        ----------
        seed : string
            The seed

        Returns
        -------
        list
            The commands generated
        """
        return Commands.FLAGS["seed"] + [str(seed)]

    @staticmethod
    def repeats(repeats):
        return Commands.FLAGS["repeats"] + [str(repeats)]


def read_timing_file(filename):
    """Handles reading .timing files.
    Currently returns the time taken as a float.

    Parameters
    ----------
    filename : string
        The timing file filename

    Returns
    -------
    float
        The run time contained in filename
    """
    with open(filename, "r") as infile:
        return float(infile.readline())


def read_embedding_file(filename):
    """Handles reading .embedding files.
    Currently returns the number of qubits used.

    Parameters
    ----------
    filename : string
        The embedding file filename

    Returns
    -------
    int
        The number of qubits used in the embedding
    """
    with open(filename, "r") as infile:
        count = 0
        for line in infile.readlines():
            qubits = line.split(":")[1]
            if qubits != "\n":
                count += len(qubits.split(","))
        return count


class Experiment(object):
    """
    Class containing all parameters for an experiment.
    """

    def __init__(self, filename):
        """Populate experiment object from a .config file.

        Parameters
        ----------
        filename
            The experiment config filename

        Returns
        -------
        Nothing
        """

        self.hardware = []
        self.programs = []
        self.program_seeds = []
        self.algorithms = []
        self.algorithm_seeds = []
        self.name = filename.replace(".config", "")

        # Read in the .experiment file
        file_dir = os.path.join(Directory.CONFIG_DIR, filename)
        with open(file_dir, "r") as infile:
            for line in infile.readlines():
                line = line.replace("\n", "")
                if line[0] == "#":
                    reading = line[1:]
                else:
                    if reading == "hardware":
                        self.hardware.append(line.split(", "))
                    elif reading == "programs":
                        graph_type, vertices, density = line.split()
                        start, stop, step = vertices.split(":")
                        for v in range(int(start), int(stop)+1, int(step)):
                            self.programs.append("%s_%d_%s" % (graph_type, v,
                                                               density))
                    elif reading == "program_seeds":
                        start, stop = line.split(":")
                        for seed in range(int(start), int(stop)+1):
                            self.program_seeds.append(str(seed))
                    elif reading == "algorithms":
                        self.algorithms.append(line)
                    elif reading == "algorithm_seeds":
                        start, stop = line.split(":")
                        for seed in range(int(start), int(stop)+1):
                            self.algorithm_seeds.append(str(seed))

    def __iter__(self):
        """Provide an iterator over the individual parameter sets.
        """
        result = []
        for params in product(self.hardware,
                              self.programs,
                              self.program_seeds,
                              self.algorithms):
            params = list(params)
            # These algorithms need seeds
            if params[-1] in ["fast-oct",
                              "fast-oct-reduce",
                              "cmr",
                              "hybrid-oct",
                              "hybrid-oct-reduce"]:
                for algorithm_seed in self.algorithm_seeds:
                    result.append(params + [algorithm_seed])
            else:
                result.append(params + ["0"])
        return iter(result)

    @staticmethod
    def convert_graph_class(graph_class):
        """Lookup for formatted program name.

        Parameters
        ----------
        graph_class : string
            The internal graph class name

        Returns
        -------
        str
            The display name for the graph class
        """
        if graph_class == "gnp":
            return "GNP"
        elif graph_class == "reg":
            return "Regular"
        elif graph_class == "powerlaw":
            return "Barabasi-Albert"
        elif graph_class == "nb":
            return "Noisy Bipartite"
        else:
            # If we don't have a nice name, return the original
            return graph_class

    @staticmethod
    def convert_density(density):
        """Lookup for formatted density name.

        Parameters
        ----------
        density : string
            The internal density name

        Returns
        -------
        str
            The display name for the density
        """
        return density.capitalize()

    @staticmethod
    def convert_algorithm(algorithm):
        """Lookup for formatted algorithm name.

        Parameters
        ----------
        algorithm : string
            The internal algorithm name

        Returns
        -------
        str
            The display name for the algorithm
        """
        if algorithm in ["fast-oct"]:
            return "Fast-OCT-Embed"
        elif algorithm in ["fast-oct-reduce"]:
            return "Reduced Fast-OCT-Embed"
        elif algorithm == "hybrid-oct":
            return "Hybrid-OCT-Embed"
        elif algorithm == "hybrid-oct-reduce":
            return "Reduced OCT-Embed"
        elif algorithm == "triad":
            return "TRIAD"
        elif algorithm in ["triad-reduce"]:
            return "Reduced TRIAD"
        elif algorithm == "cmr":
            return "CMR (Dijkstra)"
        else:
            return algorithm

    def generate_dataframe(self):
        """Populate a Pandas dataframe with an experiment's
        parameters and output.

        Returns
        -------
        Pandas dataframe
            Generate a dataframe from the internal experiment data
        """

        data = []
        for hardware, program, program_seed, algorithm in product(
                self.hardware,
                self.programs,
                self.program_seeds,
                self.algorithms):
            folder_dir = os.path.join(Directory.OUTPUT_DIR,
                                      self.name,
                                      program,
                                      program_seed,
                                      algorithm)
            try:
                file_dir = os.path.join(folder_dir, program_seed)
                row = {}
                row["Hardware"] = hardware
                row["Graph Class"], row["Vertices"], row["Density"] \
                    = program.split("_")
                row["Graph Class"] = \
                    Experiment.convert_graph_class(row["Graph Class"])
                row["Density"] = Experiment.convert_density(row["Density"])
                row["Vertices"] = int(row["Vertices"])
                row["Algorithm"] = Experiment.convert_algorithm(algorithm)
                row["Run Time"] = read_timing_file(file_dir + ".timing")
                row["Qubits Used"] = read_embedding_file(file_dir
                                                         + ".embedding")
                row["Program Seed"] = program_seed
                if row["Qubits Used"] != 0:
                    data.append(row)
            except:
                pass

        return pd.DataFrame(data)
