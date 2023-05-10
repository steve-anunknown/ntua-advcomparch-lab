import os
import matplotlib.pyplot as plt

# define the parent directory as the current directory
parent_dir = os.getcwd()

# loop through all experiment directories
for exp_dir in os.listdir(parent_dir):
    if not os.path.isdir(os.path.join(parent_dir, exp_dir)):
        continue  # skip non-directory files
    
    # loop through all subdirectories within the experiment directory
    for sub_dir in os.listdir(os.path.join(parent_dir, exp_dir)):
        if not os.path.isdir(os.path.join(parent_dir, exp_dir, sub_dir)):
            continue  # skip non-directory files
        
        # find the ipc.txt file in the subdirectory
        data_file = os.path.join(parent_dir, exp_dir, sub_dir, 'ipc.txt')
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
        
        # create a plot of the numbers
        fig, ax = plt.subplots()
        ax.plot(numbers)
        ax.set_title(sub_dir)
        ax.set_ylabel("IPC")
        
        # save the plot in the same directory as the ipc.txt file
        plot_file = os.path.join(parent_dir, exp_dir, sub_dir, '{}_{}_ipc.png'.format(exp_dir, sub_dir))
        fig.savefig(plot_file)
        plt.close(fig)

