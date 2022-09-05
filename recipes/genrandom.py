#!/usr/bin/env python3

# note that random recipes seem to be a lot less interesting than I hoped they would be
# but maybe it's a matter of tweaking the parameters just right dunno

import sys
import random

# parameters
window_size = (800, 600)
colors = ["red", "cyan", "blue", "green"]
particles_per_color = 1000
friction_range = (0.05, 0.4)
first_cut_range = (1, 7)
last_cut_range = (first_cut_range[0], first_cut_range[0] + 100)
peak_range = (0, 1)

def generate(filename):
    with open(filename, mode="w") as file:
        file.write("# randomly generated\n\n")
        file.write(f"window {window_size[0]} {window_size[1]}\n")

        friction = random.uniform(friction_range[0], friction_range[1])
        file.write(f"friction {friction}\n")

        for color in colors:
            file.write(f"particles {color} {particles_per_color}\n")

        for color1 in colors:
            for color2 in colors:
                first_cut = random.uniform(first_cut_range[0], first_cut_range[1])
                last_cut = random.uniform(last_cut_range[0], last_cut_range[1])
                peak = random.uniform(peak_range[0], peak_range[1])
                file.write(f"rule {color1} {color2} {first_cut} {last_cut} {peak}\n")

if __name__ == "__main__":
    if(len(sys.argv) != 2):
        exit

    generate(sys.argv[1])
