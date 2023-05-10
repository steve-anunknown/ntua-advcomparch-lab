import os
import matplotlib.pyplot as plt

parent_dir = os.getcwd()
subdirs = sorted(os.listdir(parent_dir))

assos = (100.0/105.0)
storg = (100.0/115.0)
adjustments = [1, assos, assos, assos,
        storg, assos*storg, assos*storg, assos*storg,
        assos*(storg**2), assos*(storg**2), assos*(storg**2)]

for sub_dir in subdirs:
    # find the ipc.txt file in the subdirectory
    data_file = os.path.join(parent_dir, sub_dir, 'ipc.txt')
    if not os.path.isfile(data_file):
        continue  # skip if ipc.txt file not found

    # read the numbers from the ipc.txt file
    with open(data_file, 'r') as f:
        numbers = []
        for line in f:
            line = line.strip()
            if line:
                try:
                    number = float(line)
                except ValueError:
                    continue  # skip non-numeric lines
                numbers.append(number)

    adjusted = [ a * b for a, b in zip(numbers,adjustments) ]
    # create a plot of the numbers
    fig, ax = plt.subplots()
    ax.plot(numbers, label="ipc normal")
    ax.plot(adjusted, label="ipc adjusted")
    ax.set_title(sub_dir)
    ax.set_ylabel("IPC")
    plt.legend()

    # save the plot in the same directory as the ipc.txt file
    plot_file = os.path.join(parent_dir, sub_dir, 'ipc_adjusted.png')
    print(plot_file)
    fig.savefig(plot_file)
    plt.close(fig)
