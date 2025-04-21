use svd_parser as svd;
use std::fs::File;
use std::io::Read;
use serde_json;

// Function to parse SVD file, returning a Result
pub fn parse_svd_file(filepath: &str) -> Result<svd_rs::Device, Box<dyn std::error::Error>> {
    let mut xml = String::new();
    File::open(filepath)?.read_to_string(&mut xml)?;
    let device = svd::parse(&mut xml)?;
    Ok(device)
}

// Function to write SVD device to JSON file, returning a Result
pub fn svd_device_to_json_file(device: &svd_rs::Device, filepath: &str) -> Result<(), Box<dyn std::error::Error>> {
    let json = serde_json::to_string_pretty(device)?;
    std::fs::write(filepath, json)?;
    Ok(())
}
