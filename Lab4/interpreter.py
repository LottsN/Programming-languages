class Interpreter:
    def get_result(listOfTokensGlobal):
            #будем запоминать последнее значение и оператор
            last_value = 0
            last_operator = ""
            print("-----------------< tokens >----------------")
            for token in listOfTokensGlobal:
                print("[ ", token.type, ": ", token.value, " ]")
                #если токен инициализации - записываем как последний оператор
                if token.type == "set":
                    last_operator = token.value
                #если токен операции - записываем как последний оператор
                elif token.type == "op":
                    last_operator = token.value
                # если токен числа - выполняем действия
                elif token.type == "num":
                    if last_operator == "set":
                        last_value = int(token.value)
                    elif last_operator == "add":
                        last_value += int(token.value)
                    elif last_operator == "sub":
                        last_value -= int(token.value)
                    elif last_operator == "mul":
                        last_value *= int(token.value)
                    elif last_operator == "div":
                        #проверка деления на ноль
                        if (token.value != "0"):
                            last_value /= int(token.value)
                        else:
                            raise Exception(f"Illegal division by zero")
                
            return int(last_value)         
            