# Streamshot

Capture snapshots from RTSP streams with flexible output and processing options.

## Features

-   Capture a single snapshot or average over an exposure time.
-   Output to file or file descriptor.
-   Support for multiple image formats and resizing options.
-   Adjustable image quality and scaling.
-   Debug modes for troubleshooting.

## Building

```bash
make build
```

## Usage

```bash
./streamshot RTSP_URL [OPTIONS]
```

### Options

| Option                         | Description                                                                                                                           |
| ------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------- |
| `-i, --input <string>`         | RTSP URL to connect to (required).                                                                                                    |
| `-t, --timeout <uint>`         | RTSP stream connection timeout in seconds (default: 10, max: 300).                                                                    |
| `-o, --output-file <string>`   | Output file path. If omitted, no file is saved.                                                                                       |
| `-O, --output-fd <uint>`       | Output file descriptor.                                                                                                               |
| `-e, --exposure <uint>`        | Exposure time in seconds (max: 86400). If omitted, snapshot is from the first I-frame; otherwise, averages frames over this time.     |
| `-f, --output-format <string>` | Output image format: `jpg`, `png`, `webp`, `tiff`, `bmp`, `ppm` (default: `jpg`).                                                     |
| `-s, --scale <float>`          | Image scale factor (0.1 to 10).                                                                                                       |
|                                | If `--scale` is set, then `--resize-height` and `--resize-width` are ignored.                                                         |
| `-h, --resize-height <uint>`   | Resize to fit specified height, maintaining aspect ratio (min: 108, max: 10800).                                                      |
| `-w, --resize-width <uint>`    | Resize to fit specified width, maintaining aspect ratio (min: 192, max: 19200).                                                       |
|                                | If both `--resize-height` and `--resize-width` are set image is resized to fit within these dimensions with maintaining aspect ratio. |
| `-q, --image-quality <uint>`   | Image quality (min: 0, max: 100, default: 95).                                                                                        |
| `-d, --debug`                  | Enable debug mode to print additional information.                                                                                    |
| `    --debug-step <uint>`      | Save debug file every N steps (default: 100, requires debug mode).                                                                    |
| `    --debug-dir <string>`     | Directory for debug files (default: `./debug_files`, requires debug mode).                                                            |
| `-h, --help`                   | Show help message and exit.                                                                                                           |
| `-v, --version`                | Show version information and exit.                                                                                                    |

### Notes

-   If neither `--output-file` nor `--output-fd` is specified, no output file is saved.
-   If `--scale`, `--resize-height`, and `--resize-width` are all omitted, the image is not resized.

## Example

```bash
./streamshot -i rtsp://my_stream.local/main --timeout=10 --output-file output.jpg --exposure 5 --output-format jpg --scale 1.0 --resize-height 480 --resize-width 640 --image-quality=95 --debug
```

This command captures a snapshot from the specified RTSP stream with a 5-second exposure, saves it as a JPEG file (`output.jpg`), resizes it to fit 480px height and 640px width, sets quality to 95, and enables debug output.
