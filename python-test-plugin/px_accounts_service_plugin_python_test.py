import pkgutil
import importlib

if pkgutil.find_loader('PluginFramework') is not None:
    PluginFramework = importlib.import_module('PluginFramework')

    class Plugin(PluginFramework.Plugin):

        title = 'python-test'

        def __init__(self):
            PluginFramework.Plugin.__init__(self)
            pass

        def verify(self, params):
            required_params  = [ 'k2' ]
            protected_params = [ 'k1' ]
            optional_params  = { 'o1': 'ov1', 'o2': 'ov2' }
            result = PluginFramework.VerifyResult()
            result.verified = True

            received_keys = []
            for k in params:
                received_keys.append(k)

            for k in required_params:
                if not k in received_keys:
                    result.verified = False
                    result.errors.append('Param \'{}\' is required'.format(k))

            if result.verified:
                for k in received_keys:
                    param = PluginFramework.ServiceParam()
                    param.key = k
                    param.val = params[k]
                    if k in required_params:
                        param.is_required = True
                    if k in protected_params:
                        param.is_protected = True
                    result.params.append(param)

                for k in protected_params:
                    if k not in received_keys:
                        param = PluginFramework.ServiceParam()
                        param.key = k
                        param.is_protected = True
                        result.params.append(param)

                for k in optional_params.keys():
                    if k not in received_keys:
                        param = PluginFramework.ServiceParam()
                        param.key = k
                        param.val = optional_params[k]
                        param.default_val = optional_params[k]
                        if k in protected_params:
                            param.is_protected = True
                        result.params.append(param)
            return result

        def authenticate(self, params):
            result = PluginFramework.AuthResult()
            token = PluginFramework.SecretToken()
            token.label = 'password'
            token.secret = 'token_value1'
            token.attributes["username"] = "k1"
            token.attributes["schema"] = "password"
            result.tokens.append(token);
            result.authenticated = True
            result.errors.append('sample warning')
            return result
