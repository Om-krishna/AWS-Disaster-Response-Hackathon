from flask import Flask, jsonify, request
app = Flask(__name__)
import requests
from flask_sqlalchemy import SQLAlchemy
db = SQLAlchemy(app)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///finedata.db'
app.config['SQLALCHEMY_TRACK_MODIFICATION'] = False
class Data(db.Model):
    GatewayId = db.Column(db.String(100), primary_key=True)
    Longitude = db.Column(db.Integer, nullable=False)
    Latittude = db.Column(db.Integer, nullable=False)
    Temperature = db.Column(db.String(80),nullable=False)
    Humadity = db.Column(db.String(80),nullable=False)
    Waterlevel = db.Column(db.String(80),nullable=False)    
    def __repr__(self):
        return f"{self.GatewayId} - {self.Longitude} - {self.Latittude} - {self.Temperature}- {self.Humadity}- {self.Waterlevel}"

@app.route('/')
def show():
    alldata = Data.query.all()
    out = []
    for dat in alldata:
        gatewaydata={
            "Gatewayid": dat.GatewayId,
            "Latitude": dat.Latittude,
            "Longitude": dat.Longitude,
            "Temperature": dat.Temperature,
            "humidity": dat.Humadity,
            "avgWaterLevel": dat.Waterlevel
            }
        out.append(gatewaydata)
    return {"data":out}

@app.route('/data', methods=['GET'])
def getdata():
    args = request.args
    if 'gatewayid' in args:
        gateway = str(args.get('gatewayid') )
    if 'temperature' in args:
        temperature = args.get('temperature') 
    if 'humadity' in args:
        humadity = args.get('humadity') 
    if 'waterlevel' in args:
        waterlevel = args.get('waterlevel')             
    print(args)
    print(gateway,temperature,humadity,waterlevel)

    url1 = "https://deploygateway.herokuapp.com/show/"+ gateway  
    print(url1)  
    response = requests.get(url1)
    check=response.json()
    print(check['data']['Onriver'])
    print("Status Code", response.status_code)
    print("JSON Response ", check)

    datt=Data.query.all()
    for das in datt:
        if(das.GatewayId == gateway):
            db.session.delete(Data.query.get(das.GatewayId))
            db.session.commit()

    GatewayId = gateway
    Longitude = check['data']['Longitude']
    Latittude = check['data']['Latitude']
    Temperature = temperature
    Humadity = humadity
    Waterlevel = waterlevel  
    dat = Data(GatewayId=GatewayId,Longitude=Longitude,Latittude=Latittude,Temperature=Temperature,Humadity=Humadity,Waterlevel=Waterlevel)
    db.session.add(dat)
    db.session.commit()
    if(check['data']['Onriver']=='YES'):
        url = "https://tgxfo7gcaa.execute-api.ap-south-1.amazonaws.com/default/postrequest"
        postdata = {
        "gateWayId": gateway,
        "Temperature": temperature,
        "humidity": humadity,
        "avgWaterLevel": waterlevel
        } 
        res = requests.post(url, json=postdata) 
        print("Status Code", res.status_code)
        print("JSON Response ", res.json())  
        print('SUCCESS')      
    else:
        print('nothing')

    return  {
    "gateWayId": gateway,
    "Temperature": temperature,
    "humidity": humadity,
    "avgWaterLevel": waterlevel
    }


if  __name__ == "__main__":
    app.run(debug=True)