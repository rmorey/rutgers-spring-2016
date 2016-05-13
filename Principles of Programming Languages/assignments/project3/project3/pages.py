# version 3:  log file name is now a command line argument.
#             You may hard code the name "log" if you prefer. See last comment
#              of file.

import sys             # version 3
import subprocess
import re
import pprint

# Calls the R system specifying that commands come from file commands.R
# The commands.R provided with this assignment will read the file named
# data and will output a histogram of that data to the file pageshist.pdf
def runR( ):
    res = subprocess.call(['R', '-f', 'commands.R'])

# log2hist analyzes a log file to calculate the total number of pages
# printed by each user during the period represented by this log file,
# and uses R to produce a pdf file pageshist.pdf showing a histogram
# of these totals.  logfilename is a string which is the name of the
# log file to analyze.
#
def log2hist(logfilename):
    with open(logfilename, 'r') as logFile:
        with open('data', 'w+') as dataFile:
            print_table = {}
            for logLine in logFile: # parse log
                tokens = logLine.split()

                # validate line
                if ('user:' not in tokens) or ('pages:' not in tokens):
                    continue
                userIndex = tokens.index('user:') + 1
                pagesIndex = tokens.index('pages:') + 1
                if (userIndex >= len(tokens) or pagesIndex >= len(tokens)):
                    continue

                # add/update user in table
                pages = int(tokens[pagesIndex])
                user = tokens[userIndex]
                print_table[user] = (print_table[user] + pages) if (user in print_table) else pages

            # write to data file
            pprint.pprint(print_table)
            for user, pages in print_table.iteritems():
                dataFile.write(str(pages) + '\n')
    runR() # create pdf

    return

if __name__ == '__main__':
    log2hist(sys.argv[1])  # version 2.

# line above may be changed to log2hist("log")
