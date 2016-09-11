{
  "targets": [
    {
      "target_name": "abema_face",
      "sources": ["../abema/abema_face_module.cpp", "../dlib/all/source.cpp"],
      'cflags': ['-fexceptions', '<!@(pkg-config --cflags opencv)'],
      'cflags_cc': ['-fexceptions'],
      'cflags!': ['-fno-exceptions'],
      'cflags_cc!': ['-fno-exception', '-fno-rtti'],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "link_settings": {
        "libraries": [
          "-L<!@(pwd)/../abema/build/dlib_build",
          "-ldlib",
          "<!@(pkg-config --libs opencv)",
        ]
      }
    }
  ]
}