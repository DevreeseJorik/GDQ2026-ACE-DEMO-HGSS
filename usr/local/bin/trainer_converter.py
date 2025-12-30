import json
import re
import struct
from pathlib import Path

_POKEMON_LIST = None

def normalize_name(name: str) -> str:
    """Lowercase, remove non-alphanumeric characters."""
    name = name.lower()
    name = re.sub(r"[^a-z0-9]", "", name)
    return name

def load_pokemon_list():
    global _POKEMON_LIST
    if _POKEMON_LIST is not None:
        return _POKEMON_LIST

    path = Path(__file__).parent / "pokemon.json"
    try:
        data = json.loads(path.read_text())
        _POKEMON_LIST = [normalize_name(n) for n in data]
        return _POKEMON_LIST
    except Exception as e:
        raise RuntimeError(f"Failed to load pokemon.json: {e}")

class TrainerConverter:
    def __init__(self, trainer_dict):
        self.data = trainer_dict or {}

    def u8(self, key):
        return int(self.data.get(key, 0)) & 0xFF

    def u16(self, key):
        return int(self.data.get(key, 0)) & 0xFFFF

    def u32(self, key):
        return int(self.data.get(key, 0)) & 0xFFFFFFFF

    def list_u16(self, key, count):
        lst = self.data.get(key, [])
        lst = lst if isinstance(lst, list) else []
        lst = lst[:count] + [0] * (count - len(lst))
        return [int(x) & 0xFFFF for x in lst]

    def resolve_species(self, value):
        try:
            return int(value)
        except Exception:
            pass

        # Case 2: string name
        if isinstance(value, str):
            norm = normalize_name(value)
            pokemon_list = load_pokemon_list()

            if norm in pokemon_list:
                return pokemon_list.index(norm)

        raise ValueError(f"Invalid species identifier: {value!r}")

    def pack_trainer_data(self, team_size=1):
        td = self.data.get("trainerData", {})

        data_type  = int(td.get("data_type", 0))
        tr_type    = int(td.get("tr_type", 0))
        tr_gra     = int(td.get("tr_gra", 0))
        poke_count = int(td.get("poke_count", team_size))

        use_item = td.get("use_item", [])
        use_item = use_item[:4] + [0] * (4 - len(use_item))

        name = td.get("name", [])

        if isinstance(name, (bytes, bytearray)):
            if len(name) > 16:
                name = name[:16]
            if len(name) < 16:
                name = name + b"\x00" * (16 - len(name))

            name_bytes = name
        else:
            if not isinstance(name, list):
                name = []

            name = name[:8] + [0] * (8 - len(name))
            name_bytes = struct.pack("<8H", *name)

        return struct.pack(
            "<BBBB"
            "4H",
            data_type, tr_type, tr_gra, poke_count,
            *use_item,
        ) + name_bytes

    def pack_poke(self, p):
        if p is None:
            p = {}

        pow_   = int(p.get("pow", 0))
        para   = int(p.get("para", 0))
        level  = int(p.get("level", 0))
        monsno = self.resolve_species(p.get("monsno", 0))
        itemno = int(p.get("itemno", 0))

        move = p.get("moves", [])
        move = move[:4] + [0] * (4 - len(move))

        custom = int(p.get("custom", 0))

        return struct.pack(
            "<BBHHHHHHH",
            pow_, para, level, monsno, itemno,
            move[0], move[1], move[2], move[3],
        ) + struct.pack("<H", custom)

    def pack(self):
        dataId = int(self.data.get("dataId", 0))

        team = self.data.get("team", [])
        trainer_data = self.pack_trainer_data(len(team))
        team = team[:6] + [None] * (6 - len(team))
        team_bin = b"".join(self.pack_poke(p) for p in team)

        return struct.pack("<H", dataId) + trainer_data + team_bin
