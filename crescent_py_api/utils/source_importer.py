import sys
import importlib
import importlib.util


class SourceImporter:
    @staticmethod
    def import_from_source(module_name: str, source_code: str, reload_module=False):
        if module_name not in sys.modules or reload_module:
            spec = importlib.util.spec_from_loader(
                module_name, loader=None, is_package=False
            )
            module = importlib.util.module_from_spec(spec)
            exec(source_code, module.__dict__)
            sys.modules[module_name] = module
        return sys.modules[module_name]
