class Helper:
    @staticmethod
    def convert_py_file_to_source_c_string(file_path: str, end_of_string=True) -> str:
        py_source_text = ""
        with open(file_path) as file:
            lines = file.readlines()
        for line in lines:
            formatted_line = line.rstrip().replace('"', '\\"').replace("\\n", "\\\\\\n")
            py_source_text += f'"{formatted_line}\\n"\\\n'
        if end_of_string:
            py_source_text += '"\\n"'
        return py_source_text.rstrip()
