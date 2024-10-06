from graphviz import Source
import os
import sys

# Ensure Graphviz executables are on the system's PATH for windows
os.environ["PATH"] += os.pathsep + "C:\\Program Files\\Graphviz\\bin\\"

def create_digraph_from_dot(dot_file):
    with open(dot_file, 'r') as f:
        dot_source = f.read()
    source = Source(dot_source, format='png')
    source.render('output', cleanup=True)  # Render the graph to a PNG file

if __name__ == "__main__":
    # Check for command-line arguments for specific dot files, otherwise, find .dot files in the directory
    dot_files = sys.argv[1:] if len(sys.argv) > 1 else [f for f in os.listdir('.') if f.endswith('.dot')]
    create_digraph_from_dot(dot_files[0])
    os.system("output.png") 
