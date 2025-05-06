import xml.etree.ElementTree as ET
from typing import Dict
import xml_helper

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

def parse_dim_index(dim_index_text, dim):
    if not dim_index_text:
        return [str(i) for i in range(dim)]
    dim_index_text = dim_index_text.strip()
    if '-' in dim_index_text:
        start, end = map(int, dim_index_text.split('-'))
        return [str(i) for i in range(start, end + 1)]
    return [x.strip() for x in dim_index_text.split(',')]

def expand_field(field_elem):
    dim_elem = field_elem.find('dim')
    if dim_elem is not None:
        dim = int(dim_elem.text)
        dim_increment = int(field_elem.find('dimIncrement').text, 0)
        dim_index = parse_dim_index(field_elem.findtext('dimIndex'), dim)
        bit_offset = int(field_elem.findtext('bitOffset'), 0)
        expanded = []
        for i in range(dim):
            idx = dim_index[i] if i < len(dim_index) else str(i)
            fld = ET.Element('field')
            for elem in list(field_elem):
                if elem.tag in ('dim', 'dimIncrement', 'dimIndex'):
                    continue
                fld.append(ET.fromstring(ET.tostring(elem)))
            # Replace %s in name/description
            for tag in ['name', 'description']:
                elem = fld.find(tag)
                if elem is not None and elem.text and '%s' in elem.text:
                    elem.text = elem.text.replace('%s', idx)
            # Set bitOffset
            bo_elem = fld.find('bitOffset')
            if bo_elem is not None:
                bo_elem.text = str(bit_offset + i * dim_increment)
            expanded.append(fld)
        return expanded
    else:
        # Not an array, just return as-is
        fld = ET.Element('field')
        for elem in list(field_elem):
            fld.append(ET.fromstring(ET.tostring(elem)))
        return [fld]

def expand_register(register_elem, base_offset=0):
    dim_elem = register_elem.find('dim')
    if dim_elem is not None:
        dim = int(dim_elem.text)
        dim_increment = int(register_elem.find('dimIncrement').text, 0)
        dim_index = parse_dim_index(register_elem.findtext('dimIndex'), dim)
        address_offset = int(register_elem.findtext('addressOffset'), 0)
        expanded = []
        for i in range(dim):
            idx = dim_index[i] if i < len(dim_index) else str(i)
            reg = ET.Element('register')
            for elem in list(register_elem):
                if elem.tag in ('dim', 'dimIncrement', 'dimIndex'):
                    continue
                if elem.tag == 'fields':
                    fields_elem = ET.Element('fields')
                    for field in list(elem):
                        for fld in expand_field(field):
                            fields_elem.append(fld)
                    reg.append(fields_elem)
                else:
                    reg.append(ET.fromstring(ET.tostring(elem)))
            for tag in ['name', 'displayName', 'description']:
                elem = reg.find(tag)
                if elem is not None and elem.text and '%s' in elem.text:
                    elem.text = elem.text.replace('%s', idx)
            addr_elem = reg.find('addressOffset')
            if addr_elem is not None:
                addr_elem.text = hex(base_offset + address_offset + i * dim_increment)
            expanded.append(reg)
        return expanded
    else:
        reg = ET.Element('register')
        for elem in list(register_elem):
            if elem.tag == 'fields':
                fields_elem = ET.Element('fields')
                for field in list(elem):
                    for fld in expand_field(field):
                        fields_elem.append(fld)
                reg.append(fields_elem)
            else:
                reg.append(ET.fromstring(ET.tostring(elem)))
        addr_elem = reg.find('addressOffset')
        if addr_elem is not None:
            addr_elem.text = hex(base_offset + int(addr_elem.text, 0))
        return [reg]

def resolve_clusters(root):
    for peripheral in root.findall('./peripherals/peripheral'):
        registers = peripheral.find('registers')
        if registers is not None:
            _resolve_clusters_in_registers(registers)

def _resolve_clusters_in_registers(registers_elem, base_offset=0):
    for child in list(registers_elem):
        if child.tag == 'register':
            expanded = expand_register(child, base_offset)
            registers_elem.remove(child)
            for reg in expanded:
                registers_elem.append(reg)
        elif child.tag == 'cluster':
            dim_elem = child.find('dim')
            cluster_offset = int(child.findtext('addressOffset'), 0)
            if dim_elem is not None:
                dim = int(dim_elem.text)
                dim_increment = int(child.find('dimIncrement').text, 0)
                for i in range(dim):
                    instance_offset = base_offset + cluster_offset + i * dim_increment
                    for sub in list(child):
                        if sub.tag == 'register':
                            expanded = expand_register(sub, instance_offset)
                            for reg in expanded:
                                registers_elem.append(reg)
                        elif sub.tag == 'cluster':
                            temp_regs = ET.Element('registers')
                            temp_regs.append(sub)
                            _resolve_clusters_in_registers(temp_regs, base_offset=instance_offset)
                            for reg in temp_regs.findall('register'):
                                registers_elem.append(reg)
                registers_elem.remove(child)
            else:
                for sub in list(child):
                    if sub.tag == 'register':
                        expanded = expand_register(sub, base_offset + cluster_offset)
                        for reg in expanded:
                            registers_elem.append(reg)
                    elif sub.tag == 'cluster':
                        temp_regs = ET.Element('registers')
                        temp_regs.append(sub)
                        _resolve_clusters_in_registers(temp_regs, base_offset=base_offset + cluster_offset)
                        for reg in temp_regs.findall('register'):
                            registers_elem.append(reg)
                registers_elem.remove(child)

def parse_svd_file(svd_file: str, debug = False) :
    peripherals = []
    tree = ET.parse(svd_file)
    root = tree.getroot()
    cleanup_text_fields(root)
    resolve_clusters(root)
    lookups = build_lookup_tables(root)
    resolve_inheritance(root, lookups)
    resolve_enumerated_values(root)
    if debug:
        print("Resolved svd stored in svd/resolved_svd.xml and svd/resolved_svd.json")
        tree.write('svd/resolved_svd.xml', encoding='utf-8', xml_declaration=True)
        xml_helper.xml_to_json_file('svd/resolved_svd.xml')
    return peripherals