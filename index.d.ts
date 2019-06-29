export declare enum MIME {
    BMP = "image/bmp",
    JPEG = "image/jpeg",
    GIF = "image/gif",
    TIFF = "image/tiff",
    PNG = "image/png"
}

export declare type ScreenshotOptions = {
    bringToFront?: boolean;
    mime?: MIME;
    grayscale?: boolean;
};

export declare function capture(
    processName: string,
    options?: ScreenshotOptions
): Promise<{ width: number; height: number; data: Buffer }>;
