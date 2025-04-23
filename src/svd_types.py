from dataclasses import dataclass
from typing import Dict, List

@dataclass
class EnumValue:
    name: str
    value: int
    description: str

@dataclass
class Field:
    name: str
    description: str
    access: str
    bit_offset: int
    bit_width: int
    enumerated_values: List[EnumValue]
    write_constraint: Dict

@dataclass
class Register:
    name: str
    display_name: str
    address_offset: int
    size: int
    access: str
    fields: List[Field]
    is_cluster: bool = False

@dataclass
class Peripheral:
    name: str = ""
    group_name: str = ""
    base_address: int = 0
    description: str = ""
    registers: List[Register] = None

@dataclass
class Device:
    peripherals: List[Peripheral] = None
    