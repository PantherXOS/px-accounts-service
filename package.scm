(use-modules
  (guix build-system cmake)
  (guix download)
  (guix gexp)
  (guix utils)
  (guix packages)
  (gnu packages linux)
  (gnu packages networking)
  (gnu packages pkg-config)
  (gnu packages python)
  (gnu packages python-xyz)
  (gnu packages serialization)
  (px packages accounts)
  ((guix licenses) #:prefix license:))


(package
  (name "px-accounts-service")
  (version "0.4.2-dev")
  (source
    (local-file (canonicalize-path ".")
                #:recursive? #t))
  (build-system cmake-build-system)
  (arguments
      `(#:tests? #f
         #:phases
         (modify-phases %standard-phases
           (add-after 'unpack 'remove-tests
             (lambda _
               (substitute* "tests/CMakeLists.txt"
                 (("test_rpc_server.cpp") "")
                 (("test_plugin_system.cpp") "")
                 (("test_event_system.cpp") ""))
               #t))
           (add-after 'install 'register-plugins
             (lambda* (#:key inputs outputs #:allow-other-keys)
               (define* (register-plugin plugin-name plugin-type #:optional (plugin-path ""))
                 (let* ((out (assoc-ref outputs "out"))
                         (python (assoc-ref inputs "python"))
                         (regpath (string-append out "/etc/px/accounts/plugins"))
                         (target (cond
                                   ((string=? plugin-type "python")
                                     (string-append out "/lib/python"
                                       ,(version-major+minor (package-version python))
                                       "/site-packages/"))
                                   ((string=? plugin-type "cpp")
                                     (string-append out "/lib/lib" plugin-name ".so"))))
                         (data (string-append "plugin:\n"
                                 "  name: " plugin-name "\n"
                                 "  version: " ,version "\n"
                                 "  type: " plugin-type "\n"
                                 "  path: " target "\n")))
                   (mkdir-p regpath)
                   (display data)
                   (with-output-to-file (string-append regpath "/" plugin-name ".yaml")
                     (lambda _ (format #t data)))
                   (if (string=? plugin-type "python")
                     (begin
                       (mkdir-p target)
                       (install-file plugin-path target)
                       (setenv "PYTHONPATH" (string-append target ":" (getenv "PYTHONPATH")))
                     ))
                   #t))
               (register-plugin (string-append ,name "-plugin-python-test") "python"
                 "src/px_accounts_service_plugin_python_test.py")
               (register-plugin (string-append ,name "-plugin-test-public-service") "python"
                 "src/px_accounts_service_plugin_test_public_service.py")
               (register-plugin (string-append ,name "-plugin-cpp-test") "cpp")
               (register-plugin (string-append ,name "-plugin-cpp-custom") "cpp")
             ))
         )))
  (inputs `(("yaml-cpp" ,yaml-cpp)
             ("capnproto" ,capnproto)))
  (native-inputs `(("pkg-config" ,pkg-config)
                    ("pybind11" ,pybind11)
                    ("nng" ,nng)
                    ("util-linux" ,util-linux "lib")))
  (propagated-inputs `(("python" ,python)
                        ("px-accounts-service-providers-mail" ,px-accounts-service-providers-mail)))
  (home-page "https://www.pantherx.org/")
  (synopsis "PantherX (Online) AccountsService")
  (description "This package provides a background services to manage
Online Accounts in PantherX")
  (license license:expat))