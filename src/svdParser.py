from dataclasses import dataclass
from typing import Dict, List
import xml.etree.ElementTree as ET
import json
from svdReader import SVDReader
from svdsuite import Parser
import pprint

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
    name: str
    base_address: int
    registers: List[Register]
    interrupts: List[Dict]

@dataclass
class Cpu:
    name: str
    core: str
    revision: str
    endian: str
    fpu_present: bool
    fpu_type: str
    mpu_present: bool
    mpu_type: str
    dcache_present: bool
    icache_present: bool
    features: List[str]
    interrupts: List[Dict]

@dataclass
class Device:
    version: str = "unknown"
    cpu: Cpu = None
    peripherals: List[Peripheral] = None
    interrupts: List[Dict] = None
    


def parse_peripheral(peripheral_element) -> Peripheral:
    pass

def parse_svd(svd_file: str) -> Device:
    tree = ET.parse(svd_file)
    root = tree.getroot()
    device : Device = Device()

    device.version = root.find('version').text
    return device
    
if __name__ == "__main__":
    SVD_FILE = 'svd/stm32f767.svd'
    process = Parser.from_svd_file(SVD_FILE)
    device = process.get_parsed_device()
    # pprint to file
    with open('parsed_device.json', 'w') as f:
        pprint.pprint(device, stream=f, indent=4)
    
    
    
