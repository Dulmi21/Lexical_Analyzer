import re


class Token:
    def __init__(self, type, value):
        self.type = type
        self.value = value


class LexicalAnalyzer:
    def __init__(self, text):
        self.text = text
        self.pos = 0
        self.tokens = []

    def remove_comments_and_spaces(self):
        # Remove comments
        self.text = re.sub(r"#.*$", "", self.text, flags=re.MULTILINE)
        # Remove spaces and newlines
        self.text = re.sub(r"\s+", " ", self.text)

        # Tokenization loop
        patterns = [
            (r"\b(let|in|fn)\b", "KEYWORD"),
            (r"\b[a-zA-Z_]\w*\b", "IDENTIFIER"),
            (r"\d+", "NUMBER"),
            (r'"([^"\\]|\\.)*"', "STRING"),
            (r'[+\-*/<>&.@/:=˜|$#!%^_\[\]{}"‘?]+', "OPERATOR"),
            (r"[(),;]", "PUNCTUATION"),
            (r"\s+", "SPACE"),
        ]

        pos = 0
        while pos < len(self.text):
            match = None
            for pattern, token_type in patterns:
                regex = re.compile(pattern)
                match = regex.match(self.text, pos)
                if match:
                    value = match.group(0)
                    if token_type != "SPACE":  # Skip adding tokens of type 'SPACE'
                        token = Token(token_type, value)
                        self.tokens.append(token)
                    pos = match.end()
                    break
            if not match:
                raise Exception("Unexpected character: " + self.text[pos:])

    def check_reserved_keywords(self):
        reserved_keywords = {"let", "in", "fn"}
        for token in self.tokens:
            if token.value in reserved_keywords:
                token.type = "KEYWORD"

    def analyze(self):
        self.remove_comments_and_spaces()
        self.check_reserved_keywords()
        return self.tokens


# Test
text = """
# This is a comment
let x = 42 in fn y => x + y
This is a commment let Sum(A) = Psum (A,Order A )where rec Psum (T,N) = N eq 0 -> 0 | Psum(T,N-1)+T Nin Print ( Sum (1,2,3,4,5)
"""
lexer = LexicalAnalyzer(text)
tokens = lexer.analyze()
for token in tokens:
    print(token.type, ",", token.value)
