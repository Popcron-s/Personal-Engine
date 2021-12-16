#pragma once

namespace _GRAPH{
	enum class TYPE{
		NONE = 0,
		STOP,
		PAUSE,
		PLAY
	};
	enum class INTERPOLATION{
		POINT,
		CLIFF,
		LINEAR,
		SQUARED,
		INVSQURED
	};
};

namespace _MODULE{
	enum class TYPE : unsigned int{
		NONE = 0x00000000,
		HIGH_ORDER = 0xFFFF0000,
		Mesh = 0x00010000,
		MPoint = 0x00010001,
		MLine = 0x00010002,
		MPolygon = 0x00010003,
		MText = 0x00010004,
		Collider = 0x00020000,
		CPolygon = 0x00020001,
		CCircle = 0x00020002,
		Physicsbody = 0x00030000,
		Rigidbody = 0x00030001,
		Softbody = 0x00030002,
		Sound = 0x00040000,
		Animation = 0x00080000,

		Camera = 0x00090000
	};
};

namespace _PROJECTION{
	enum class TYPE{
		Ortho,
		Perspective
	};
}

namespace _COLLISION{
	enum class TYPE{
		Ignore,
		Overlap,
		Block
	};
}

namespace _RGIDBODY{
	enum class TYPE{
		Static,
		Dynamic
	};
}

namespace RESOURCEFORMAT{
	enum class TYPE{
		UNKNOWN,
		IMAGE,
		SOUND,
		FONT,
		TEXTURE,
		SHADER,
		TEXT
	};
}

namespace _UNIFORM{
	enum class TYPE{
		CAMERA_BUF,
		CAMERA_HEAD,

		TEXTURE
	};
}

namespace _SHADER{
	enum class TYPE{
		VERTEX,
		GEOMETRY,
		FRAGMENT
	};
}

namespace LIBRARY_TYPE{
	enum class PLATFORM{
		Win32,
		Win64,
		Android,
		iOS
	};

	enum class GRAPHIC{
		NONE,
		OpenGL,	//PC - openGL, Mobile - openGL ES
		DX9,
		DX11
	};

	enum class SOUND{
		NONE,
		OpenSL,
		XAudio2
	};
}