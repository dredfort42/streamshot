/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | errors.h
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-16

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef ERRORS_H
#define ERRORS_H

#define ERROR_FAILED_TO_ALLOCATE_MEMORY "Error: Failed to allocate memory."
#define ERROR_FAILED_TO_CREATE_DEBUG_DIR "Error: Failed to create debug directory."
#define ERROR_FAILED_TO_OPEN_FILE "Error: Failed to open file."
#define ERROR_FAILED_TO_WRITE_FILE "Error: Failed to write to file."
#define ERROR_FAILED_TO_WRITE_FD "Error: Failed to write to file descriptor."
#define ERROR_NO_DATA_TO_WRITE "Error: No data to write to file descriptor."
#define ERROR_NOT_NULL_TERMINATED "Error: The provided message is not null-terminated."
#define ERROR_FAILED_TO_GET_TIME "Error: Failed to get the current time."

#define ERROR_INVALID_ARGUMENTS "Error: Invalid arguments provided."
#define ERROR_INVALID_RTSP_URL "Error: Invalid RTSP URL provided."
#define ERROR_INVALID_OUTPUT_FD "Error: Invalid output file descriptor specified."
#define ERROR_INVALID_TIMEOUT "Error: Invalid timeout value."
#define ERROR_INVALID_EXPOSURE "Error: Invalid exposure value."
#define ERROR_INVALID_OUTPUT_FORMAT "Error: Invalid output format specified."
#define ERROR_INVALID_SCALE_FACTOR "Error: Invalid scale factor specified."
#define ERROR_INVALID_RESIZE_HEIGHT "Error: Invalid resize height specified."
#define ERROR_INVALID_RESIZE_WIDTH "Error: Invalid resize width specified."
#define ERROR_INVALID_IMAGE_QUALITY "Error: Invalid image quality specified."
#define ERROR_INVALID_DEBUG_STEP "Error: Invalid debug step specified."
#define ERROR_INVALID_DEBUG_DIR "Error: Invalid debug directory specified."

#define ERROR_FAILED_TO_SET_STREAM_OPTIONS "Error: Failed to set stream options."
#define ERROR_FAILED_TO_OPEN_RTSP_STREAM "Error: Failed to open RTSP stream."
#define ERROR_FAILED_TO_GET_STREAM_INFO "Error: Failed to get stream information."
#define ERROR_NO_CODEC_PARAMETERS_FOUND "Error: No codec parameters found for video stream."
#define ERROR_INVALID_DESTINATION_DIMENSIONS "Error: Invalid destination dimensions for scaling."
#define ERROR_FAILED_TO_CREATE_SWS_CONTEXT "Error: Failed to create SwsContext for scaling."

#define ERROR_INVALID_FPS "Error: Invalid FPS value specified."

#define ERROR_FAILED_TO_GET_IMAGE_SIZE "Error: Failed to get image size."
#define ERROR_NO_STREAMS_FOUND "Error: No streams found in the format context."
#define ERROR_NO_VIDEO_STREAM_FOUND "Error: No video stream found in the format context."
#define ERROR_NO_DECODER_FOUND "Error: No decoder found for the video stream."
#define ERROR_FAILED_TO_ALLOCATE_CODEC_CONTEXT "Error: Failed to allocate codec context."
#define ERROR_FAILED_TO_ALLOCATE_VIDEO_FRAME "Error: Failed to allocate video frame."
#define ERROR_FAILED_TO_ALLOCATE_IMAGE_FRAME "Error: Failed to allocate image frame."
#define ERROR_FAILED_TO_ALLOCATE_BUFFER "Error: Failed to allocate buffer for image data."
#define ERROR_FAILED_TO_ALLOCATE_PACKET "Error: Failed to allocate AVPacket."
#define ERROR_FAILED_TO_ALLOCATE_RAW_IMAGE "Error: Failed to allocate raw image structure."
#define ERROR_FAILED_TO_ALLOCATE_SUM_BUFFER "Error: Failed to allocate sum buffer."
#define ERROR_FAILED_TO_COPY_CODEC_PARAMETERS "Error: Failed to copy codec parameters to context."
#define ERROR_FAILED_TO_OPEN_CODEC "Error: Failed to open codec."
#define ERROR_FAILED_TO_FILL_IMAGE_ARRAYS "Error: Failed to fill image arrays."
#define ERROR_FAILED_TO_SEND_PACKET "Error: Failed to send packet to decoder."
#define ERROR_FAILED_TO_RECEIVE_FRAME "Error: Failed to receive frame from decoder."
// #define ERROR_FAILED_TO_SEND_FRAME "Error: Failed to send frame to codec context."

#define RTN_ERROR -1
#define RTN_SUCCESS 0

#define MAIN_ERROR_CODE 1
#define MAIN_SUCCESS_CODE 0

#endif  // ERRORS_H