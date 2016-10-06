{
  "targets": [
    {
      "target_name": "abema_face",
      "sources": ["../abema/abema_face_module.cpp", "../dlib/all/source.cpp"],
      'cflags': ['-fexceptions', '<!@(pkg-config --cflags opencv)', '-O3', '-frtti'],
      'cflags_cc': ['-fexceptions', '-O3', '-frtti'],
      'cflags_cc+': ['-frtti'],
      'cflags!': ['-fno-exceptions'],
      'cflags_cc!': ['-fno-exception', '-fno-rtti'],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "link_settings": {
        "libraries": [
          "-lpthread",
          "-lopenblas",
          "-llapack",
          "<!@(pkg-config --libs opencv)",
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