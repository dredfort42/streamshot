# Streamshot

Capture snapshots from RTSP streams with flexible output and processing options.

## Features

-   Capture a single snapshot or average over an exposure time.
-   Output to file or file descriptor.
-   Support for multiple image formats and resizing options.
-   Adjustable image quality and scaling.
-   Verbose and debug modes for troubleshooting.

## Building

```bash
make build
```

## Usage

```bash
./streamshot STREAM_URL [OPTIONS]
```

### Options

| Option          | Description                                                                                                                       |
| --------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `-t <uint>`     | RTSP stream connection timeout in seconds (default: 10, max: 300).                                                                |
| `-ofl <string>` | Output file path. If omitted, no file is saved.                                                                                   |
| `-ofd <uint>`   | Output file descriptor. Takes precedence over `-ofl` if both are specified.                                                       |
| `-e <uint>`     | Exposure time in seconds (max: 86400). If omitted, snapshot is from the first I-frame; otherwise, averages frames over this time. |
| `-f <string>`   | Image format: `jpg`, `png`, `webp`, `tiff`, `bmp`, `ppm` (default: `jpg`).                                                        |
| `-s <float>`    | Image scale factor (0.1 to 10).                                                                                                   |
| `-stfh <uint>`  | Resize to fit specified height, maintaining aspect ratio.                                                                         |
| `-stfw <uint>`  | Resize to fit specified width, maintaining aspect ratio.                                                                          |
|                 | If `-s` is set, it overrides `-stfh` and `-stfw`. If both `-stfh` and `-stfw` are set, both dimensions are considered.            |
| `-q <uint>`     | Image quality (1–100, default: 95).                                                                                               |
| `-vm`           | Verbose mode: print additional information.                                                                                       |
| `-d`            | Debug mode: print debug information.                                                                                              |
| `-sdfns <uint>` | Save debug file every N steps (default: 100, requires debug mode).                                                                |
| `-dff <string>` | Debug files folder (default: `./debug_files`, requires debug mode).                                                               |
| `-h`            | Show help message and exit.                                                                                                       |
| `-v`            | Show version information and exit.                                                                                                |

### Notes

-   If neither `-ofl` nor `-ofd` is specified, no output file is saved.
-   If `-s`, `-stfh`, and `-stfw` are all omitted, the image is not resized.

## Example

```bash
./streamshot rtsp://my_stream.local/main -t 10 -ofl output.jpg -e 5 -f jpg -s 1.0 -stfh 480 -stfw 640 -q 95 -v
```

This command captures a snapshot from the specified RTSP stream with a 5-second exposure, saves it as a JPEG file (`output.jpg`), resizes it to fit 480px height and 640px width, sets quality to 95, and enables verbose output.
