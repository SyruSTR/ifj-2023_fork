from test_runtime import ErrorType, test, nr_failed


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

if __name__ == "__main__":
    test_all()
    exit(nr_failed)