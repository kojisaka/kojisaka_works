package com.example.demo;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.PostMapping;

@Controller
public class DispDB_boot {

	final static String Sql
		= "SELECT name,type,GROUP_CONCAT(shared) "
			+ "FROM dep_status GROUP BY name,type";

	@PostMapping("/dispDB")
	public String disp(Model m) {
		List<String> rl = new ArrayList<String>();
		Connection con = null;
		Statement stmt = null;
		ResultSet rs = null;
		try {
			con = DriverManager.getConnection(DBProps.url,
												DBProps.user, DBProps.pass);
			stmt = con.createStatement();
			rs = stmt.executeQuery(Sql);
			while (rs.next()) {
				String name   = rs.getString("name");
				String type   = rs.getString("type");
				String detail = rs.getString("GROUP_CONCAT(shared)");
				rl.add(name + "　" + type + "　" + detail);
			}
		} catch(Exception e) {
			e.getStackTrace();
		} finally {
			try {
				if (rs   != null) rs.close();
				if (stmt != null) stmt.close();
				if (con  != null) con.close();
			} catch(Exception e) {
				e.getStackTrace();
			}
		}
		m.addAttribute("rslt_list", rl);
		return "dispDB.html";
	}
}
