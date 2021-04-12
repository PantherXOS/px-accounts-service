import pkgutil
import importlib

if pkgutil.find_loader('PluginFramework') is not None:
    PluginFramework = importlib.import_module('PluginFramework')


    class Plugin(PluginFramework.Plugin):
        title = 'protected-test'

        def __init__(self):
            PluginFramework.Plugin.__init__(self)
            pass


        def verify(self, params):
            required_params = ['param1', 'param2']
            protected_params = ['param1', 'param2']
            result = PluginFramework.VerifyResult()
            result.verified = True
            for k in required_params:
                if k not in params:
                    result.verified = False
                    result.errors.append('param \'{}\' is required.'.format(k))
            if not result.verified:
                return result
            for k in params:
                prm = PluginFramework.ServiceParam()
                prm.key = k
                prm.val = params[k]
                if k in protected_params:
                    prm.is_protected = True
                result.params.append(prm)
            return result


        def authenticate(self, params):
            result = PluginFramework.AuthResult()
            result.authenticated = True
            return result
