from test_runtime import ErrorType, test, nr_failed


def test_all():
    test("let a = 5ssss","",expected_return_code=ErrorType.error_undefined_var)

if __name__ == "__main__":
    test_all()
    exit(nr_failed)