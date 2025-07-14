from test_runtime import ErrorType, test, nr_failed


def test_invalid(code, expected_return_code):
    test(code, "", expected_return_code=expected_return_code)

def test_parse_fail(code: str):
    test(code, "", expected_return_code=ErrorType.error_parser)

def test_all():
    # check rule
    test("let a = 5ssss","",expected_return_code=ErrorType.error_undefined_var)

    # infinity recursion :_(
    test("nil ?? 4","")

    # Compiler SIGSEGV
    test_invalid("""
    var b: Int = 10

    func test(_ _int: Int){
        return
    }
    
    test(b,b)
    """,ErrorType.error_call)

    test_invalid("""
    var abcd: Int

    write( ab
    ""","",ErrorType.error_parser)

    # infinity recursion :_(
    test("""
    Double

    var a :Int 
    ""","",expected_return_code=ErrorType.error_parser)

    # infinity recursion :_(
    test("""
        func a() {
        return}
        }
    ""","", expected_return_code=ErrorType.error_parser)

    # check in LSP
    test_invalid("""
    func a(_ a: Int) {}
    let value: Int? = 4
    a(value)
    """, ErrorType.error_type)

    test_invalid("""
    func a(_ i: Int?,) {}
    a(nil)
    """, ErrorType.error_parser)

    # is it parser error?
    test_invalid("""
    func a() -> String {
        return 
        return 
    }
    """,ErrorType.error_parser)

    test("""
    let a = 5
    let b = "ahoj"
    let c = a + b
    """, "", expected_return_code=ErrorType.error_type)

    test("ord(3, 4)", "", expected_return_code=ErrorType.error_call)

    test("length(42)", "", expected_return_code=ErrorType.error_call)

    test_parse_fail("let a : kentus")

    test("""
    // Program 2: Vypocet faktorialu (rekurzivne)
    // Hlavni telo programu
    write("Zadejte cislo pro vypocet faktorialu: ")
    let inp = readInt()

    // pomocna funkce pro dekrementaci celeho cisla o zadane cislo
    func decrement(of n: Int, by m: Int) -> Int {
        return n - m
    }

    // Definice funkce pro vypocet hodnoty faktorialu
    func factorial(_ n : Int) -> Int {
        var result : Int?
        if (n < 2) {
            result = 1
        } else {
            let decremented_n = decrement(of: n, by: 1)
            let temp_result = factorial(decremented_n)
            result = n * temp_result
        }
        return result!
    }
    // pokracovani hlavniho tela programu
    if let inp {
        if (inp < 0) { // Pokracovani hlavniho tela programu
            write("Faktorial nelze spocitat!")
        } else {
            let vysl = factorial(inp)
            write("Vysledek je: ", vysl)
        }
    } else {
        write("Chyba pri nacitani celeho cisla!")
    }
    """, "Zadejte cislo pro vypocet faktorialu: Vysledek je: 120", program_stdin="5\n")


if __name__ == "__main__":
    test_all()
    exit(nr_failed)