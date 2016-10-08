{
  "targets": [
    {
      "target_name": "face2abema",
      "sources": ["addons/abema_face_module.cpp", "addons/dlib/all/source.cpp"],
      'cflags': ['-fexceptions', '<!@(pkg-config --cflags opencv)'],
      'cflags_cc': ['-fexceptions', '<!@(pkg-config --cflags opencv)'],
      'cflags!': ['-fno-exceptions', '-fno-rtti'],
      'cflags_cc!': ['-fno-exception', '-fno-rtti'],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "link_settings": {
        "libraries": [
          "-lpthread",
          "-lopenblas",
          "-llapack",
          "<!@(pkg-config --libs opencv)"
        ]
      },
      "defines": [
        'DLIB_NO_GUI_SUPPORT',
        'DLIB_USE_BLAS',
        'DLIB_USE_LAPACK'
      ]
    }
  ]
}