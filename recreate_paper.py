"""
Recreates the paper results
"""


import argparse
from subprocess import call


def write_small():
    """Write the small.config file
    """
    with open("data/config/small.config", "w") as outfile:
        outfile.write("#hardware\n")
        outfile.write("4, 4, 4\n")
        outfile.write("#programs\n")
        outfile.write("nb 2:32:2 low\n")
        outfile.write("nb 2:32:2 medium\n")
        outfile.write("nb 2:32:2 high\n")
        outfile.write("gnp 2:32:2 low\n")
        outfile.write("gnp 2:32:2 medium\n")
        outfile.write("gnp 2:32:2 high\n")
        outfile.write("reg 2:32:2 low\n")
        outfile.write("reg 2:32:2 medium\n")
        outfile.write("reg 2:32:2 high\n")
        outfile.write("powerlaw 2:32:2 low\n")
        outfile.write("powerlaw 2:32:2 medium\n")
        outfile.write("powerlaw 2:32:2 high\n")
        outfile.write("#program_seeds\n")
        outfile.write("0:24\n")
        outfile.write("#algorithms\n")
        outfile.write("triad\n")
        outfile.write("triad-reduce\n")
        outfile.write("fast-oct\n")
        outfile.write("fast-oct-reduce\n")
        outfile.write("#algorithm_seeds\n")
        outfile.write("0:9\n")


def write_medium():
    """Write the medium.config file
    """
    with open("data/config/medium.config", "w") as outfile:
        outfile.write("#hardware\n")
        outfile.write("4, 8, 8\n")
        outfile.write("#programs\n")
        outfile.write("nb 2:64:2 low\n")
        outfile.write("nb 2:64:2 medium\n")
        outfile.write("nb 2:64:2 high\n")
        outfile.write("gnp 2:64:2 low\n")
        outfile.write("gnp 2:64:2 medium\n")
        outfile.write("gnp 2:64:2 high\n")
        outfile.write("reg 2:64:2 low\n")
        outfile.write("reg 2:64:2 medium\n")
        outfile.write("reg 2:64:2 high\n")
        outfile.write("powerlaw 2:64:2 low\n")
        outfile.write("powerlaw 2:64:2 medium\n")
        outfile.write("powerlaw 2:64:2 high\n")
        outfile.write("#program_seeds\n")
        outfile.write("0:24\n")
        outfile.write("#algorithms\n")
        outfile.write("triad\n")
        outfile.write("triad-reduce\n")
        outfile.write("fast-oct\n")
        outfile.write("fast-oct-reduce\n")
        outfile.write("#algorithm_seeds\n")
        outfile.write("0:9\n")


def write_large():
    """Write the large.config file
    """
    with open("data/config/large.config", "w") as outfile:
        outfile.write("#hardware\n")
        outfile.write("4, 16, 16\n")
        outfile.write("#programs\n")
        outfile.write("nb 2:128:2 low\n")
        outfile.write("nb 2:128:2 medium\n")
        outfile.write("nb 2:128:2 high\n")
        outfile.write("gnp 2:128:2 low\n")
        outfile.write("gnp 2:128:2 medium\n")
        outfile.write("gnp 2:128:2 high\n")
        outfile.write("reg 2:128:2 low\n")
        outfile.write("reg 2:128:2 medium\n")
        outfile.write("reg 2:128:2 high\n")
        outfile.write("powerlaw 2:128:2 low\n")
        outfile.write("powerlaw 2:128:2 medium\n")
        outfile.write("powerlaw 2:128:2 high\n")
        outfile.write("#program_seeds\n")
        outfile.write("0:24\n")
        outfile.write("#algorithms\n")
        outfile.write("triad\n")
        outfile.write("triad-reduce\n")
        outfile.write("fast-oct\n")
        outfile.write("fast-oct-reduce\n")
        outfile.write("#algorithm_seeds\n")
        outfile.write("0:9\n")


def write_cmr():
    """Write the cmr.config file
    """
    with open("data/config/cmr.config", "w") as outfile:
        outfile.write("#hardware\n")
        outfile.write("4, 8, 8\n")
        outfile.write("#programs\n")
        outfile.write("gnp 2:64:2 low\n")
        outfile.write("gnp 2:64:2 medium\n")
        outfile.write("gnp 2:64:2 high\n")
        outfile.write("#program_seeds\n")
        outfile.write("0:9\n")
        outfile.write("#algorithms\n")
        outfile.write("cmr\n")
        outfile.write("fast-oct-reduce\n")
        outfile.write("#algorithm_seeds\n")
        outfile.write("0:9\n")


def write_hybrid():
    """Write the hybrid.config file
    """
    with open("data/config/hybrid.config", "w") as outfile:
        outfile.write("#hardware\n")
        outfile.write("4, 8, 8\n")
        outfile.write("#programs\n")
        outfile.write("gnp 2:64:2 low\n")
        outfile.write("gnp 2:64:2 medium\n")
        outfile.write("gnp 2:64:2 high\n")
        outfile.write("#program_seeds\n")
        outfile.write("0:9\n")
        outfile.write("#algorithms\n")
        outfile.write("hybrid-oct\n")
        outfile.write("fast-oct\n")
        outfile.write("#algorithm_seeds\n")
        outfile.write("0:9\n")


if __name__ == "__main__":
    """Main method for CLI interaction.

    Parameters
    ----------
    num_threads : int
        The number of CPU threads to use
    experiment : string
        An experiment from [small, medium, large, cmr, hybrid]

    Returns
    -------
    Nothing
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("num_threads",
                        help="The number of compute threads used.",
                        type=int)
    parser.add_argument("--experiment",
                        help="Run experiment from [small, medium, large, cmr,\
                        hybrid]. Runs medium if not specified",
                        type=str)
    args = parser.parse_args()

    # Choose the appropriate config file
    config = ""
    if args.experiment:
        if args.experiment == "small":
            write_small()
            config = "small.config"
        elif args.experiment == "medium":
            write_medium()
            config = "medium.config"
        elif args.experiment == "large":
            write_large()
            config = "large.config"
        elif args.experiment == "cmr":
            write_cmr()
            config = "cmr.config"
        elif args.experiment == "hybrid":
            write_hybrid()
            config = "hybrid.config"
    else:
        write_medium()
        config = "medium.config"

    # Set up the script commands
    experiment_command = ["python",
                          "scripts/experiment.py",
                          config,
                          str(args.num_threads)]

    plot_command = ["python",
                    "scripts/plot.py",
                    config]

    # Call the scripts
    call(experiment_command)
    call(plot_command)
