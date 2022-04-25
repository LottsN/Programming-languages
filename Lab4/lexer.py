from tokens import Token

class Lexer:
    def generate_tokens( text):
        # лист токенов
        tokensList = []
        elementNum = 0

        for el in text:
            # если сет - то токен сет
            if el == "set":
                newElement = Token("set", el)
            # если число - то токен нам
            elif el.isdigit():
                newElement = Token("num", el)
            # если операция - то токен операции
            elif (el == "add" or el == "div" or el == "sub" or el == "mul"):
                newElement = Token("op", el)
            else:
                raise Exception(f"Illegal character '{el}' at position '{elementNum}'. Try usind numbers or one of the commands: set, add, mul, div, sub")
            tokensList.append(newElement)
            elementNum += 1
        tokensList.append(Token("end", None))
        return tokensList
