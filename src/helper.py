def replace_values(data, target_key, replace_map):
    """
    Recursively replaces values of `target_key` in nested dicts/lists using `replace_map`.
    
    Args:
        data: Input data (dict, list, or other)
        target_key: Key whose values should be replaced
        replace_map: Dictionary mapping old values to new values
    
    Returns:
        Modified data with replacements applied
    """
    if isinstance(data, dict):
        return {
            k: replace_values(v, target_key, replace_map) if k != target_key 
            else replace_map.get(v, v)  # Replace only if key matches
            for k, v in data.items()
        }
    elif isinstance(data, list):
        return [replace_values(item, target_key, replace_map) for item in data]
    else:
        return data
    

def test_replace_values():
    # Original data with nested structure
    data = {
        "size": 32,
        "nested": {
            "size": 16,
            "other_key": "unchanged",
            "list": [
                {"size": 8},
                {"deep": {"size": 64}}
            ]
        }
    }

    # Replacement map: size (bits) → C++ type
    replace_map = {
        8: "uint8_t",
        16: "uint16_t",
        32: "uint32_t",
        64: "uint64_t"
    }

    # Replace all "size" values
    modified_data = replace_values(data, "size", replace_map)
    assert modified_data == {
        "size": "uint32_t",
        "nested": {
            "size": "uint16_t",
            "other_key": "unchanged",
            "list": [
                {"size": "uint8_t"},
                {"deep": {"size": "uint64_t"}}
            ]
        }
    }


