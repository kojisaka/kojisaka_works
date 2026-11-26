const TypeBus = "Bus";
const TypeFC  = "FC";
const PortNo = 80;
const express = require("express");
const app = express();
const htmlReg  = require("fs").readFileSync("./regMem.html" , "utf8");
const htmlDisp = require("fs").readFileSync("./dispMem.html", "utf8");

var dss = [];

app.use(express.urlencoded({ extended: true }));

app.get("/regMem", (req, res) => {
	sendRegHtml(res, "", "", "", "");
});

app.post("/regMem", (req, res) => {
	var name   = req.body.name  ;
	var type   = req.body.type  ;
	var detail = req.body.detail;
	var msg;
	if (name === null || name.match(".*[\0-/].*")) {
		msg = "名前の入力が正しくありません！";
	} else if (type === TypeBus) {
		if (! detail.match("[0-9]+")) {
			msg = "利用者数の入力が正しくありません！";
		} else {
			var num = Number.parseInt(detail);
			var ds = new BusDepStatus(name, type, num);
			dss.push(ds);
			msg = name + " を登録しました。";
			detail = type = name = "";
		}
	} else if (type === TypeFC) {
		var cs = detail.trim().split(" +");
		var ds = new FCDepStatus(name, type, cs);
		dss.push(ds);
		msg = name + " を登録しました。";
		detail = type = name = "";
	} else {
		msg = "業種の入力が正しくありません！";
	}
	sendRegHtml(res, name, type, detail, msg);
});

function sendRegHtml(res, name, type, detail, msg) {
	res.type("html").send(htmlReg.replaceAll("${name}"  , name  )
								 .replaceAll("${type}"  , type  )
								 .replaceAll("${detail}", detail)
								 .replaceAll("${msg}"   , msg   ));
}

app.post("/dispMem", (req, res) => {
	var num_of_rslt = dss.length;
	var rslt_list = "";
	for (const ds of dss) {
		rslt_list += "\t" + ds.getStatus() + "<br>\n";
	}
	res.type("html").send(htmlDisp.replaceAll("${num_of_rslt}", num_of_rslt)
								  .replaceAll("${rslt_list}"  , rslt_list  ));
});

class DepStatus {

	name;
	type;

	constructor(n, t) {
		this.name = n;
		this.type = t;
	}

	getStatus() {
		return null;
	}
}

class BusDepStatus extends DepStatus {

	numOfPAX;

	constructor(n, t, num) {
		super(n, t);
		this.numOfPAX = num;
	}

	getStatus() {
		return this.name + "　バス　利用者数: " + this.numOfPAX;
	}
}

class FCDepStatus extends DepStatus {

	chains = new Array(0);

	constructor(n, t, cs) {
		super(n, t);
		this.chains = cs;
	}

	getStatus() {
		var status = this.name + "　FC　店一覧: ";
		for (const chain of this.chains) {
			status += (chain + " ");
		}
		return status;
	}
}

app.listen(PortNo, () => console.log("Listening on port " + PortNo))
