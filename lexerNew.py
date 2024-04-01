# implementinga lexical analyzer

# import the python module for regular expressions
import re

# RPAL's Lexicans

token_patterns = [
    ("IDENTIFIER", r"[a-zA-Z_][a-zA-Z0-9_]*"),
    ("INTEGER", r"\d+"),
    ("OPERATOR", r'[+\-*/<>&.@/:=˜|$#!%^_\[\]{}"‘?]+'),
    ("STRING", r"'(\\t|\\n|\\'|\\\\|[\(\),;\w\s])*'"),
    ("SPACE", r"\s+"),
    ("PUNCTUATION", r"[(),;]"),
]

# compile regural expression patterns

compiled_patterns = []

for name, pattern in token_patterns:
    compiled_patterns.append((name, re.compile(pattern)))

# lexer definition


def lexical_analyzer(string_In):
    # intiate a list to store tokens of the input list
    tokens_list = []

    # iterate through input string
    while string_In:
        match = None  # store the match object returned from the re match operation

        for name, pattern in compiled_patterns:
            match = pattern.match(string_In)
            if match:
                if name != "SPACE":  # spaces are removed
                    # -----Remove comments-------------#

                    tokens_list.append((name, match.group(0)))
                string_In = string_In[
                    match.end() :
                ]  # removing the matched portion from the beginning

                break
        if not match:  # No valid token at the current position of the input string
            raise Exception(f"Invalid token: {input_string}")
    return tokens_list


input_string = "This is a commment let Sum(A) = Psum (A,Order A )where rec Psum (T,N) = N eq 0 -> 0 | Psum(T,N-1)+T Nin Print ( Sum (1,2,3,4,5) "
tokens = lexical_analyzer(input_string)
for item in tokens:
    print(item)
