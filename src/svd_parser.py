import xml.etree.ElementTree as ET
from typing import Dict, List, Optional
from typing import Tuple
import xml_helper
import json

def cleanup_text_fields(element: ET.Element) -> None:
    ''' Removes \n and trailing spaces from text fields in the XML element tree. '''
    for elem in element.iter():
        if elem.text:
            elem.text = ' '.join(elem.text.split())

def build_lookup_tables(root : ET.Element) :
    ''' Builds lookup tables for peripherals, registers, and fields. '''
    peripherals = {}
    registers = {}
    fields = {}
    for peripheral in root.findall('./peripherals/peripheral'):
        pname = peripheral.findtext('name')
        peripherals[(pname,)] = peripheral
        for register in peripheral.findall('registers/register'):
            rname = register.findtext('name')
            registers[(pname, rname)] = register
            registers[(rname,)] = register # Some derivedFrom uses relative paths
            for field in register.findall('fields/field'):
                fname = field.findtext('name')
                fields[(pname, rname, fname)] = field
                fields[(fname,)] = field # Some derivedFrom uses relative paths
    return peripherals, registers, fields

def resolve_inheritance(root : ET.Element, lookups ):
    ''' Resolves inheritance (derivedFrom) in the SVD XML file. '''
    lut_peripherals, lut_registers, lut_fields = lookups

    def resolve_element_inheritance(element: ET.Element, lookup: Dict[str, ET.Element]) -> None:
        ''' Resolves inheritance for a given element. '''
        derived_from = element.get('derivedFrom')
        if derived_from is not None:
            key = tuple(derived_from.split('.'))
            base_element = lookup.get(key)
            if base_element is not None:
                # Merge attributes from base element to derived element
                for key, value in base_element.attrib.items():
                    if key not in element.attrib:
                        element.set(key, value)
                # Merge child elements from base element to derived element
                for child in base_element:
                    tag_name = child.tag
                    existing_child = element.find(tag_name)
                    if existing_child is None:
                        element.append(ET.fromstring(ET.tostring(child).decode()))
                    else: # resolve nested inheritance
                        resolve_element_inheritance(existing_child, {}) # TODO: is this correct? necessary?
                element.attrib.pop('derivedFrom')
            else:    
                print(f"Warning: Element '{element.tag}' with name '{element.findtext('name')}' references non-existent base element '{derived_from}'")

        # Recursively resolve inheritance for child elements (registers, fields).
        if element.tag == 'peripheral':
            for register in element.findall('registers/register'):
                rname = register.findtext('name')
                if rname is not None:
                    resolve_element_inheritance(register, lut_registers)
        elif element.tag == 'register':
            for field in element.findall('fields/field'):
                fname = field.findtext('name')
                if fname is not None:
                    resolve_element_inheritance(field, lut_fields)
    
    # Loop through all peripherals and resolve inheritance
    for peripheral in root.findall('./peripherals/peripheral'):
        resolve_element_inheritance(peripheral, lut_peripherals)

def resolve_enumerated_values(root : ET.Element) -> None:
    for register in root.findall('./peripherals/peripheral/registers/register'):
        register.append(ET.Element('enums'))
        for field in register.findall('fields/field'):
            for enum_values in field.findall('enumeratedValues'):
                name = enum_values.findtext('name')
                if name is None:
                    name = enum_values.get('derivedFrom')
                
                # create a element called enumName
                enum_name = ET.Element('enumName')
                enum_name.text = name
                field.append(enum_name)

                register.find('enums').append(enum_values)
                field.remove(enum_values)

def parse_svd_file(svd_file: str) :
    peripherals = []
    tree = ET.parse(svd_file)
    root = tree.getroot()
    cleanup_text_fields(root)
    lookups = build_lookup_tables(root)
    resolve_inheritance(root, lookups)
    resolve_enumerated_values(root)
    # Save resolved result to file
    tree.write('svd/resolved_svd.xml', encoding='utf-8', xml_declaration=True)

    xml_helper.xml_to_json_file('svd/resolved_svd.xml')

    
    return peripherals