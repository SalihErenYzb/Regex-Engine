from graphviz import Digraph
import re
import os
## TODO: turn this to basic application with textbox given regex automatically shows the graph
## TODO: Make application show the dfs every step as the regex match runs for particular string

# Ensure Graphviz executables are on the system's PATH
os.environ["PATH"] += os.pathsep + "C:\\Program Files\\Graphviz\\bin\\"

def parse_edges(file_path):
    edges = []
    with open(file_path, 'r') as file:
        for line in file:
            # Use regex to parse each line in the format "src char -> dest char [color=color]"
            match = re.match(r'(\d+) (\S+) -> (\d+) (\S+) \[color=(\w+)\];', line.strip())
            if match:
                src = int(match.group(1))
                src_char = match.group(2)
                dest = int(match.group(3))
                dest_char = match.group(4)
                color = match.group(5)
                edges.append((src, dest, src_char, dest_char, color))
        
        # Change the last edge so that its dest_char is "end"
        if edges:
            src, dest, src_char, dest_char, color = edges[-1]
            edges[-1] = (src, dest, src_char, 'end', color)

    return edges

def create_digraph(edges):
    # Sort edges based on the source node
    edges.sort(key=lambda x: x[0])

    # Create a directed graph
    dot = Digraph()
    dot.attr(rankdir='LR')  # Set the layout direction to Left-to-Right

    # Add nodes and edges to the graph
    nodes = {}
    for edge in edges:
        src, dest, src_char, dest_char, color = edge

        # Add node for the source if it doesn't exist
        if src not in nodes:
            dot.node(str(src), f'{src}: {src_char}')
            nodes[src] = True

        # Add node for the destination if it doesn't exist
        if dest not in nodes:
            dot.node(str(dest), f'{dest}: {dest_char}')
            nodes[dest] = True

        # Add edge with color
        dot.edge(str(src), str(dest), color=color)

    # Save the graph to a file (PNG format)
    dot.render('output', format='png')

if __name__ == "__main__":
    # Parse the edges from input.txt
    edges = parse_edges("C:\\Users\\algos\\OneDrive\\Masaüstü\\regex\\grapht.txt")

    # Create the directed graph
    create_digraph(edges)
    ## open the output.png file to see the graph
    os.system("output.png")
