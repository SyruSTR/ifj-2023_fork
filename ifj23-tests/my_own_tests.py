from test_runtime import ErrorType, test, nr_failed


def test_invalid(code, expected_return_code):
    test(code, "", expected_return_code=expected_return_code)

def test_all():
    # check rule
    test("let a = 5ssss","",expected_return_code=ErrorType.error_undefined_var)

    # infinity recursion :_(
    test("nil ?? 4","")

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

if __name__ == "__main__":
    test_all()
    exit(nr_failed)