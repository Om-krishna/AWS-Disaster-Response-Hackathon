from geopy.geocoders import Nominatim
import json
from flask import Flask, jsonify, request, render_template
app = Flask(__name__)
from flask_sqlalchemy import SQLAlchemy
db = SQLAlchemy(app)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///savedata.db'
app.config['SQLALCHEMY_TRACK_MODIFICATION'] = False
class Data(db.Model):
    Id = db.Column(db.Integer, primary_key=True)
    Longitude = db.Column(db.Integer, nullable=False)
    Latittude = db.Column(db.Integer, nullable=False)
    Areaname = db.Column(db.String(80),nullable=False)
    City = db.Column(db.String(80),nullable=False)
    State = db.Column(db.String(80),nullable=False)
    Pincode = db.Column(db.Integer,nullable=False)
    Country = db.Column(db.String(80),nullable=False)
    
    def __repr__(self):
        return f"{self.Id} - {self.Onriver} - {self.Longitude} - {self.Latittude} - {self.Areaname}- {self.City}- {self.State}- {self.Pincode}- {self.Country}"

@app.route('/', methods=['GET','POST'])
def index():
    return render_template('index.html')

@app.route('/data', methods=['POST'])
def store():
    output = request.get_json()
    result = json.loads(output) #this converts the json output to a python dictionary

    Longitude = result['Longitude']
    Latitude = result['Latitude']
    cord = (Latitude,Longitude)
    geoLoc = Nominatim(user_agent="GetLoc")
    locname = geoLoc.reverse(cord)
    add = locname.address
    myadd = add.split(",")
    dicadd = {i:myadd[i] for i in range(len(myadd))}
    print(dicadd)
    area= myadd[0]
    city= myadd[-4]
    state= myadd[-3]
    pin= myadd[-2]
    country= myadd[-1]
    print(dicadd)
    da = Data(Longitude=Longitude,Latittude=Latitude,Areaname=area,City=city,State=state,Pincode=pin,Country=country)
    db.session.add(da)
    db.session.commit()
    return "succes"

@app.route('/showSaveMeRequest', methods=['GET'])
def show():
    alldata = Data.query.all()
    out = []
    for dat in alldata:
        gatewaydata={
            "id": dat.Id,
            "Latitude": dat.Latittude,
            "Longitude": dat.Longitude,
            "Area name": dat.Areaname,
            "City": dat.City,
            "State": dat.State,
            "Pincode": dat.Pincode,
            "Country": dat.Country
            }
        out.append(gatewaydata)
    return {"data":out}

    
if  __name__ == "__main__":
    app.run(debug=True)