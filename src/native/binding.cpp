#include <Windows.h>
#include <napi.h>
#include <atlimage.h>
#include <thread>
#include <chrono>
#include "ScreenShot.h"
#include "FindProcess.h"

using namespace ScreenShot;
using namespace Napi;
using namespace std;

Object CreateImageData(Env env, Buffer<BYTE> buffer, INT width, INT height) {
	Object returnValue = Object::New(env);
	returnValue.Set("buffer", buffer);
	returnValue.Set("width", Number::New(env, width));
	returnValue.Set("height", Number::New(env, height));
	return returnValue;
}

Promise Capture(const CallbackInfo& info) {
	string windowTitle = info[0].IsUndefined() ? "" : info[0].As<String>().Utf8Value();
	Promise::Deferred deferred = Promise::Deferred::New(info.Env());

	if (windowTitle == "") {
		deferred.Reject(
			Napi::TypeError::New(info.Env(), "No process name given.").Value()
		);
		return deferred.Promise();
	}

	HWND windowHWND = FindProcessMainWindowByName(windowTitle.c_str());

	if (windowHWND == NULL) {
		string exceptionMessage = "Could not find window for process name: ";
		exceptionMessage += windowTitle;
		deferred.Reject(
			Napi::TypeError::New(info.Env(), exceptionMessage).Value()
		);
		return deferred.Promise();
	}

	Object options = info.Length() > 1 ? info[1].As<Object>() : Object::New(info.Env());
	BOOLEAN grayscale = options.Get("grayscale").IsUndefined() ? false : options.Get("grayscale").As<Boolean>() == true;
	BOOLEAN bringToFront = options.Get("bringToFront").IsUndefined() ? true : options.Get("bringToFront").As<Boolean>() == true;
	const string mime = options.Get("mime").IsUndefined() ? "image/png" : options.Get("mime").As<String>().Utf8Value();

	if (mime != "image/jpeg" && mime != "image/png" && mime != "image/tiff" && mime != "image/bmp" && mime != "image/gif") {
		deferred.Reject(
			Napi::TypeError::New(info.Env(), "Invalid mime type give.").Value()
		);
		return deferred.Promise();
	}

	if (bringToFront) {
		if (!BringToFront(windowHWND)) {
			deferred.Reject(
				Napi::TypeError::New(info.Env(), "Failed to bring process to front.").Value()
			);
			return deferred.Promise();
		}

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
	
	RECT targetWindowBounds = GetWindowBounds(windowHWND);
	BYTE* buffer;
	const DWORD bufferLen = GetScreenshotBuffer(buffer, targetWindowBounds, grayscale, mime);

	if (bufferLen < 1) {
		deferred.Reject(
			Napi::TypeError::New(info.Env(), "Buffer storage failed.").Value()
		);
		return deferred.Promise();
	}

	Buffer<BYTE> byteBuffer = Buffer<BYTE>::New(info.Env(), buffer, bufferLen);

	deferred.Resolve(
		CreateImageData(info.Env(), byteBuffer, targetWindowBounds.right - targetWindowBounds.left, targetWindowBounds.bottom - targetWindowBounds.top)
	);

	return deferred.Promise();
}

Promise CaptureAsImageData(const CallbackInfo& info) {
	return Capture(info);
}

Object Init(Env env, Object exports) {
	exports.Set(Napi::String::New(env, "capture"), Function::New(env, CaptureAsImageData));

	return exports;
}

NODE_API_MODULE(addon, Init)