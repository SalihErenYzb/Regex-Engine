This is my own Regex engine implementation that is not meant to be used in production. It is mostly for education purposes. I'm also trying to make it very efficient.

I also created a simple visualizer for the NFA the regex represents in Python below image represents the digraph created by the regex pattern "(a+b+c+d+|patternnn|samosamo)*visually".

![output](https://github.com/user-attachments/assets/67c1cb0b-24d9-4120-aaf4-8e39d9ee6c25)


TODO: Try to find a way to do dfs without using unordered_set for efficiency

TODO: Make it possible to init giving a map of special characters like (,|,*,+,) etc.

TODO: Make code cleaner and more readable

TODO: Add support for [a-z] and [0-9] etc.

TODO: Add support for ^ and $ for start and end of string

TODO: Add support for \d, \w, \s etc.

TODO: Add support for ?, :, or {} etc.

TODO: Add function to find all matches 

TODO: Make it possible to reach substrings inside () after matches.

TODO: Return the matching string which allows indexing for every () in the regex????

TODO: Turn the visualizer into a small application that show the graphs DFS over characters of a text.

