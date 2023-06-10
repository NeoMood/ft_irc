import requests
import json
import os
import base64

client_id = os.getenv('CLIENT_ID')
secret_id = os.getenv('SECRET_ID')
auth_url = 'https://accounts.spotify.com/api/token'
auth_options = {
    'Authorization': 'Basic ' + base64.b64encode((client_id + ':' + secret_id).encode('utf-8')).decode('utf-8')
}
auth_data = {
    'grant_type': 'client_credentials'
}

def authenticate():
    try:
        response = requests.post(url=auth_url, headers=auth_options, data=auth_data)
        if response.status_code == 200:
            token = response.json().get('access_token')
            return token
    except requests.exceptions.InvalidSchema as e:
        print(e)

def get_album():
    token = authenticate()

if __name__ == '__main__':
    authenticate()