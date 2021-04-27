# vim: et ts=4 sw=4
import os
from flask import Flask, escape, request, render_template, abort


app = Flask(__name__)

commands = {
    'a-on'  : 'doas brenn a on',
    'a-off' : 'doas brenn a off',
    'b-on'  : 'doas brenn b on',
    'b-off' : 'doas brenn b off',
    'c-on'  : 'doas brenn c on',
    'c-off' : 'doas brenn c off',
    'd-on'  : 'doas brenn d on',
    'd-off' : 'doas brenn d off',
}


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/run/<message>')
def run(message):
    try:
        command     = commands[message] + ' > /dev/null'
        return_code = os.system(command)

        if return_code != 0:
            abort(500)

        return ''

    except KeyError:
        abort(404)

    except:
        abort(500)

