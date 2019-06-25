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
	const string mime = options.Get("mime").IsUndefined() ? "image/jpeg" : options.Get("mime").As<String>().Utf8Value();

	if (mime != "image/jpeg" && mime != "image/png" && mime != "image/tiff" && mime != "image/bmp" && mime != "image/gif") {
		deferred.Reject(
			Napi::TypeError::New(info.Env(), "Invalid mime type give.").Value()
		);
		return deferred.Promise();
	}

	if (bringToFront) {
		if (!SetForegroundWindow(windowHWND)) {
			deferred.Reject(
				Napi::TypeError::New(info.Env(), "Failed to bring process to front.").Value()
			);
			return deferred.Promise();
		}

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	RECT targetWindowBounds = GetWindowBounds(windowHWND);
	CHAR* buffer;
	const DWORD bufferLen = CaptureAsBuffer(buffer, targetWindowBounds, grayscale, mime);

	if (bufferLen < 1) {
		deferred.Reject(
			Napi::TypeError::New(info.Env(), "Buffer storage failed.").Value()
		);
		return deferred.Promise();
	}
	Object returnValue = Object::New(info.Env());
	returnValue.Set("data", Buffer<CHAR>::New(info.Env(), buffer, bufferLen));
	returnValue.Set("width", Number::New(info.Env(), targetWindowBounds.right - targetWindowBounds.left));
	returnValue.Set("height", Number::New(info.Env(), targetWindowBounds.bottom - targetWindowBounds.top));

	deferred.Resolve(returnValue);

	return deferred.Promise();
}

Object Init(Env env, Object exports) {
	exports.Set(Napi::String::New(env, "capture"), Function::New(env, Capture));

	return exports;
}

NODE_API_MODULE(addon, Init)