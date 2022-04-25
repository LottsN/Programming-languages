from lexer import Lexer
from interpreter import Interpreter

def main():
    try:
        #получаем текст программы
        text = open('program.txt', 'r').read()

        print("--------------< program text >-------------")
        print(text)

        #из текста получаем токены с помощью лексера(анализатора лексики)
        list_of_tokens =Lexer.generate_tokens(text.split())

        #полученные токены отдает интерпретатору для выполнения программы
        result = Interpreter.get_result(list_of_tokens)

        #выводим результат
        print("-----------------< result >----------------")
        print(result, "\n")

    except Exception as e:
        print(e)

main()