from flask import Flask, jsonify
app = Flask(__name__)

@app.get('/')
def index():
  stuff = {'name': 'alice','email': 'alice@outlook.com'}
  return jsonify(stuff)
  
@app.route('/bruh')
def bruh():
  return '<p>bruh!</p>'

if __name__ == '__main__':
  app.run(debug=True)

