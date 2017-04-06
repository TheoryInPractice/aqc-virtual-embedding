"""
Script for generating plots from experiment output.
"""


import argparse
import os
import numpy as np
import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt
from utilities import Directory, Experiment


def print_full(data):
    """Prints a dataframe at full width.
    Useful for peeking at the data for debugging.

    Parameters
    ----------
    data : Pandas dataframe
        The experimental results

    Returns
    -------
    Nothing
    """
    pd.set_option('display.max_rows',
                  len(data),
                  "display.max_columns",
                  500,
                  "display.width",
                  1000)
    print(data)
    pd.reset_option('display.max_rows')


def plot_grid(experiment, data, y_value, filename):
    """Plot a Seaborn facet grid of the results.
    Rows are programs, columns are density levels.

    Parameters
    ----------
    experiment : Experiment
        The experiment parameters
    data : Pandas dataframe
        The experimental results
    y_value : string
        One of "Qubits Used" or "Run Time"
    filename : string
        The output filename

    Returns
    -------
    Nothing
    """

    sns.set(style="darkgrid")
    plot = sns.FacetGrid(data=data,
                         row="Graph Class",
                         col="Density",
                         margin_titles=True)
    plot = plot.map_dataframe(sns.tsplot,
                              time="Vertices",
                              value=y_value,
                              err_style=None,
                              unit="Program Seed",
                              condition="Algorithm",
                              color="deep",
                              estimator=np.nanmedian)
    plt.subplots_adjust(top=0.9)

    # Clear off row titles and set main title
    for ax in plot.axes.flat:
        plt.setp(ax.texts, text="")

    # Format the axes (and y-scale) properly
    x_label = "Vertices"
    y_label = y_value
    if y_value == "Run Time":
        y_label = "Run time (sec)"
        for ax in plot.axes.flat:
            ax.set_yscale("log")
    elif y_value == "Qubits Used":
        y_label = "Qubits used (lower is better)"
    plot.set_axis_labels(x_label, y_label)

    # Set axis ranges
    xlimit = data[["Vertices"]].max()[0]
    ylimit = data[[y_value]].max()[0]
    if y_label == "Run time (sec)":
        plot.set(xlim=(0, 1.05*xlimit), ylim=(0, 70000))
    else:
        plot.set(xlim=(0, 1.05*xlimit), ylim=(0, 1.05*ylimit))

    # Place legend and titles, then save
    plot.fig.get_axes()[0].legend(loc="upper left")
    plot.set_titles(row_template="{row_name}",
                    col_template="{col_name} Density",
                    fontweight="bold",
                    fontsize=20)
    plot.savefig(filename)


if __name__ == "__main__":
    """Main method for CLI interaction.

    Parameters
    ----------
    experiment_filename : string
        The experiment config filename

    Returns
    -------
    Nothing
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("experiment_filename",
                        help="The experiment config file name.",
                        type=str)
    args = parser.parse_args()

    # Load the data
    filename = args.experiment_filename
    experiment = Experiment(filename)
    experiment_name = filename.replace(".config", "")
    data = experiment.generate_dataframe()
    print("Creating plots for '{}'".format(experiment_name))

    # Create the directory if needed
    working_dir = os.path.join(Directory.PLOTS_DIR, experiment_name)
    if not os.path.exists(working_dir):
        os.makedirs(working_dir)

    # Generate grid plots
    plot_grid(experiment,
              data,
              "Qubits Used",
              os.path.join(working_dir, "Qubits_Used.pdf"))
    print("Wrote Qubits_Used.pdf to {}/".format(working_dir))

    plot_grid(experiment,
              data,
              "Run Time",
              os.path.join(working_dir, "Run_Time.pdf"))
    print("Wrote Run_Time.pdf to {}/".format(working_dir))
