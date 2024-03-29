using namespace Gdiplus;

namespace ScreenShot {
	RECT GetWindowBounds(HWND hwnd) {
		RECT rc;

		GetWindowRect(hwnd, &rc);

		return rc;
	}

	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT num = 0;          // number of image encoders
		UINT size = 0;         // size of the image encoder array in bytes

		ImageCodecInfo* pImageCodecInfo = NULL;

		GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}

	BOOLEAN ConvertBitmapToGrayscale(HBITMAP hbitmap, HDC hdc)
	{
		BITMAP bm;
		GetObject(hbitmap, sizeof(bm), &bm);
		if (bm.bmBitsPixel < 24)
		{
			return FALSE;
		}

		DWORD size = ((bm.bmWidth * bm.bmBitsPixel + 31) / 32) * 4 * bm.bmHeight;

		BITMAPINFO bmi
		{ sizeof(BITMAPINFOHEADER), bm.bmWidth, bm.bmHeight, 1, bm.bmBitsPixel, BI_RGB, size };

		int stride = bm.bmWidth + (bm.bmWidth * bm.bmBitsPixel / 8) % 4;
		BYTE *bits = new BYTE[size];
		GetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
		for (int y = 0; y < bm.bmHeight; y++) {
			for (int x = 0; x < stride; x++) {
				int i = (x + y * stride) * bm.bmBitsPixel / 8;
				BYTE gray = BYTE(0.299 * bits[i + 0] + 0.587 * bits[i + 1] + 0.114 * bits[i + 2]);
				bits[i + 0] = bits[i + 1] = bits[i + 2] = gray;
			}
		}
		SetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
		delete[]bits;

		return TRUE;
	}

	DWORD WriteImage(IStream *pStream, BYTE*& buffer) {
		LARGE_INTEGER liZero = {};
		ULARGE_INTEGER pos = {};
		STATSTG stg = {};
		DWORD bytesRead = 0;
		HRESULT hrRet = S_OK;

		DWORD dwBufferSize = 0;

		hrRet = pStream->Seek(liZero, STREAM_SEEK_SET, &pos);
		hrRet = pStream->Stat(&stg, STATFLAG_NONAME);

		buffer = new BYTE[stg.cbSize.LowPart];
		hrRet = (buffer == NULL) ? E_OUTOFMEMORY : S_OK;
		dwBufferSize = stg.cbSize.LowPart;

		hrRet = pStream->Read(buffer, stg.cbSize.LowPart, &bytesRead);
		pStream->Release();

		return dwBufferSize;
	}


	std::wstring StringToWString(const std::string& s)
	{
		std::wstring temp(s.length(), L' ');
		std::copy(s.begin(), s.end(), temp.begin());
		return temp;
	}

	DWORD GetScreenshotBuffer(BYTE*& buffer, RECT targetWindowBounds, BOOL useGrayscale, const std::string mime) {
		using namespace Gdiplus;
		IStream* istream;
		HRESULT res = CreateStreamOnHGlobal(NULL, true, &istream);
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		DWORD dwBufferSize;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		{
			HDC scrdc, memdc;
			HBITMAP membit;
			scrdc = ::GetDC(0);
			CLSID clsid;
			memdc = CreateCompatibleDC(scrdc);
			membit = CreateCompatibleBitmap(scrdc, targetWindowBounds.right - targetWindowBounds.left,
				targetWindowBounds.bottom - targetWindowBounds.top);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
			BitBlt(memdc, 0, 0, targetWindowBounds.right - targetWindowBounds.left,
				targetWindowBounds.bottom - targetWindowBounds.top, scrdc, targetWindowBounds.left, targetWindowBounds.top, SRCCOPY);

			if (useGrayscale && !ConvertBitmapToGrayscale(membit, scrdc)) {
				return 0;
			}

			Gdiplus::Bitmap bitmap(membit, NULL);
		
			GetEncoderClsid(StringToWString(mime).c_str(), &clsid);
			CreateStreamOnHGlobal(NULL, TRUE, &istream);

			bitmap.Save(istream, &clsid, NULL);
			dwBufferSize = WriteImage(istream, buffer);
			

			DeleteObject(memdc);
			DeleteObject(membit);
			::ReleaseDC(0, scrdc);
		}

		GdiplusShutdown(gdiplusToken);

		return dwBufferSize;
	}
}