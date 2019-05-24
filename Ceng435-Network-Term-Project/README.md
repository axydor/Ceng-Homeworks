Python3 should be used for running the scripts.
We send a file from source to node in this implementation.
Each python files should be run on a device associated with its filename.
First broker then r1, r2 and destination scripts should be run without any argument.
After these you should run the source.py on the source with filename argument which is the file that you want to send to destination.
After the file is sent, in the source, output file will be generated which includes end-to-end delay for each packet with size 8.
To run the experiments you have to add the delays to the corresponding links in the topology and run the experiment.

