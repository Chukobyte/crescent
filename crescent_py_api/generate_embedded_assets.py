import base64
from typing import List, Optional


class AssetType:
    TEXT = 0
    FONT = 1


class EmbeddedAsset:
    def __init__(self, name: str, asset_type: int, file_path: str) -> None:
        self.name = name
        self.asset_type = asset_type
        self.file_path = file_path


class EmbedAssetUtil:
    BASE_FOLDER = "assets"

    @staticmethod
    def generate_asset_header(file_path: str, assets: List[EmbeddedAsset]) -> None:
        file_string = "#pragma once\n"
        file_string += "\n"
        file_string += "#include <stddef.h>\n"
        file_string += "\n"
        for asset in assets:
            file_string += EmbedAssetUtil._generate_file_hex_array(asset)

        with open(file_path, 'w') as header_file:
            header_file.write(file_string)

    @staticmethod
    def _generate_file_hex_array(asset: EmbeddedAsset, hex_postfix="_HEX", str_postfix="_STR", size_postfix="_SIZE") -> str:
        with open(asset.file_path, 'rb') as file:
            file_content = file.read()
            hex_array = f"const unsigned char {asset.name}{hex_postfix}[] = {{\n"
            hex_array += ', '.join([f'0x{byte:02x}' for byte in file_content])
            hex_array += "\n"
            hex_array += "};\n"
            hex_array += "\n"
            hex_array += f"#define {asset.name}{str_postfix} (const char*){asset.name}{hex_postfix}\n"
            hex_array += "\n"
            hex_array += f"const size_t {asset.name}{size_postfix} = {len(file_content)};\n\n"
            return hex_array


EmbedAssetUtil.generate_asset_header("embedded_assets.h", [
    EmbeddedAsset(
        name="CRE_EMBEDDED_ASSET_FONT_VERDANA_TTF",
        asset_type=AssetType.FONT,
        file_path="../assets/fonts/verdana.ttf",
    )
])
