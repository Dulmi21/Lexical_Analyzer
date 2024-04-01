# Token types
token_types = {
    "IDENTIFIER": lambda char: char.isalpha() or char == "_",
    "INTEGER": lambda char: char.isdigit(),
    "OPERATOR": lambda char: char in '+-*/<>&.@/:=˜|$#!%^_[{}]"',
    "STRING": lambda char: char == "'",
    "SPACE": lambda char: char.isspace(),
    "PUNCTUATION": lambda char: char in ",;()",
}


# Lexer definition
def lexical_analyzer(input_string):
    tokens_list = []
    i = 0

    while i < len(input_string):
        char = input_string[i]

        # Skipping spaces
        if token_types["SPACE"](char):
            i += 1
            continue

        # Identifying token types
        for token_type, condition in token_types.items():
            if condition(char):
                if token_type == "STRING":
                    string_end = input_string.find("'", i + 1)
                    if string_end == -1:
                        raise Exception("Invalid string: missing closing quote")
                    tokens_list.append((token_type, input_string[i : string_end + 1]))
                    i = string_end + 1
                elif token_type == "OPERATOR":
                    operator_end = i + 1
                    while operator_end < len(input_string) and token_types[token_type](
                        input_string[i : operator_end + 1]
                    ):
                        operator_end += 1
                    tokens_list.append((token_type, input_string[i:operator_end]))
                    i = operator_end
                else:
                    token_end = i + 1
                    while token_end < len(input_string) and token_types[token_type](
                        input_string[token_end]
                    ):
                        token_end += 1
                    tokens_list.append((token_type, input_string[i:token_end]))
                    i = token_end
                break

        # If no match is found, raise an exception
        else:
            raise Exception(f"Invalid token: {input_string[i:i+10]}")

    return tokens_list


# Example usage
input_string = "This is a comment let Sum(A) = Psum (A,Order A )where rec Psum (T,N) = N eq 0 -> 0 | Psum(T,N-1)+T Nin Print ( Sum (1,2,3,4,5) "
tokens = lexical_analyzer(input_string)
for item in tokens:
    print(item)
