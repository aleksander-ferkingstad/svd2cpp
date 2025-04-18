use svd_parser as svd;
use std::env::args;
use std::fs::File;
use std::io::Read;
use std::path::PathBuf;
use std::io::Write;  // Add this import

fn main() {
    let mut args = args();
    let svd_fn = if let (Some(_), Some(arg1), None) = (args.next(), args.next(), args.next()) {
        PathBuf::from(arg1)
    } else {
        println!("Usage: (svd2json) file.svd");
        return;
    };

    let mut svd_xml = String::new();
    File::open(&svd_fn)
        .expect("Failed to open SVD input file")
        .read_to_string(&mut svd_xml)
        .expect("Failed to read SVD input file to a String");

    let device = svd::parse(&mut svd_xml).expect("Failed to parse the SVD file into Rust structs");

    // Convert to JSON
    let json = serde_json::to_string_pretty(&device)
        .expect("Failed to serialize device to JSON");
        
    // Create output filename
    let mut json_fn = svd_fn.clone();
    json_fn.set_extension("json");
    
    // Write to file
    let mut file = File::create(&json_fn)
        .expect("Failed to create output JSON file");
    file.write_all(json.as_bytes())
        .expect("Failed to write JSON to file");
    
    println!("Successfully converted to {}", json_fn.display());
}
