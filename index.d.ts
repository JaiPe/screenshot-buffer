export declare function capture(
    filename: string,
    options: {
        bringToFront?: boolean;
        grayscale?: boolean;
        mime?: MIME;
    }
): Promise<{ width: number; height: number; data: Buffer }>;

export declare const enum MIME {
    BMP = "image/bmp",
    JPEG = "image/jpeg",
    GIF = "image/gif",
    TIFF = "image/tiff",
    PNG = "image/png"
}
