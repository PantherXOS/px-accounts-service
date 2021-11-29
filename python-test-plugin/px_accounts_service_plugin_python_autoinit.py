import pkgutil
import importlib

if pkgutil.find_loader('PluginFramework') is not None:
    PluginFramework = importlib.import_module('PluginFramework')

    class Plugin(PluginFramework.Plugin):

        title = 'python-autoinit'
        auto_init = True
        max_count = 1

        def __init__(self):
            PluginFramework.Plugin.__init__(self)
            pass

        def verify(self, params):
            optional_params  = { 'test-key': 'test-value' }
            result = PluginFramework.VerifyResult()
            result.verified = True

            received_keys = []
            for k in params:
                received_keys.append(k)

            for key in optional_params.keys():
                if key not in received_keys:
                    param = PluginFramework.ServiceParam()
                    param.key = key
                    param.val = optional_params[key]
                    result.params.append(param)
            return result

        def authenticate(self, _params):
            result = PluginFramework.AuthResult()
            result.authenticated = True
            return result
